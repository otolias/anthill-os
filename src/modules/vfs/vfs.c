#include "vfs.h"

#include <errno.h>
#include <fcall.h>
#include <pstring.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/vfs.h>

static mqd_t mq_in = -1;
static struct vnode *root;
static unsigned long id_count = 0;;
static unsigned tag_count = 0;
static unsigned fid_count = 0;

/*
* Allocate space for a child of _vnode_
*/
static struct vnode* _add_child(struct vnode *vnode) {
    /* Check if more space is needed */
    unsigned n = 1;
    while (n < vnode->children_no) n *= 2;

    if (vnode->children_no == 0) {
        vnode->children = malloc(4 * sizeof(*vnode->children));
        if (!vnode->children) return NULL;
    } else if (n > 4 && vnode->children_no + 1 > n) {
        void *temp = realloc(vnode->children, n * 2);
        if (!temp) return NULL;

        vnode->children = temp;
    }

    vnode->children_no++;
    return &vnode->children[vnode->children_no - 1];
}

/*
* Generate a Twalk message for the driver
*/
static enum vfs_error _expand(struct vnode *mount, pstring *elements, unsigned short n) {
    unsigned new_fid = fid_count++;

    struct vfs_msg vfs_msg;
    vfs_msg.fcall.type = Twalk;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = mount->fid;
    vfs_msg.fcall.newfid = new_fid;
    vfs_msg.fcall.nwname = n;
    vfs_msg.fcall.wname = elements;
    vfs_msg.mq_id = mq_in;

    if (!vfs_msg_send(&vfs_msg, mount->mq_id, mq_in))
        return VFS_MQERR;

    if (vfs_msg.fcall.type == Rerror)
        return VFS_MQERR;

    struct vnode *vnode = mount;
    pstring *element = elements;
    for (unsigned short i = 0; i < vfs_msg.fcall.nwqid; i++) {
        struct vnode *child = vnode_find_child(vnode, element);
        if (!child) {
            child = _add_child(vnode);
            if (!child) return VFS_NOMEM;

            child->name = pstrdup(NULL, element, 0);
            if (!child->name)
                { child = vnode_remove(child); child = NULL; return VFS_NOMEM; }
            memcpy(&child->qid, &vfs_msg.fcall.wqid[i], sizeof(struct qid));

            child->qid.id = id_count++;
            child->qid.type = vfs_msg.fcall.wqid[i].type;
            child->qid.version = 0;
            child->children = NULL;
            child->children_no = 0;
            child->mount_node = vnode->mount_node;
            child->mq_id = vnode->mq_id;
        }

        element = pstriter(element);
        vnode = child;
    }

    vnode->fid = new_fid;

    return VFS_OK;
}

struct vnode* vnode_find_child(const struct vnode* vnode, const pstring *name) {
    for (unsigned i = 0; i < vnode->children_no; i++) {
        if (!vnode->children[i].name) continue;

        if (memcmp(name->s, vnode->children[i].name->s, name->len) == 0) {
            return &vnode->children[i];
        }
    }

    return NULL;
}

struct vnode* vnode_get_root() {
    return root;
}

unsigned vnode_forward(const struct vnode *vnode, struct vfs_msg *vfs_msg, char *buf) {
    unsigned len;
    vfs_msg->fcall.tag = tag_count++;
    vfs_msg->fcall.fid = vnode->fid;
    vfs_msg->mq_id = mq_in;

    len = vfs_msg_put(vfs_msg, buf);
    if (len == 0)
        return 0;

    if (mq_send(vnode->mq_id, buf, len, 0) == -1)
        return 0;

    if (mq_receive(mq_in, buf, VFS_MAX_MSG_LEN, 0) == -1)
        return 0;

    if (vfs_msg_parse(vfs_msg, buf) == 0)
        return 0;

    len = vfs_msg_parse(vfs_msg, buf);

    return len;
}

struct vnode* vnode_remove(struct vnode *node) {
    if (!node) return NULL;

    if (node->children) {
        for (unsigned i = 0; i < node->children_no; i++)
            node->children = vnode_remove(&node->children[i]);

        free(node->children); node->children = NULL;
    }

    if (node->name) { free(node->name); node->name = NULL; }
    free(node);
    return NULL;
}

unsigned short vnode_scan(pstring *elements, unsigned short n, struct qid *wqid,
                          struct vnode **node) {
    struct vnode *mount = root;
    struct vnode *vnode = mount;
    pstring *mount_element = elements;
    pstring *element = mount_element;
    unsigned short i = 0, mount_index = 0;

    while (i < n) {
        if (vnode->mount_node == NULL) {
            mount = vnode;
            mount_element = element;
            mount_index = i;
        }

        wqid[i] = vnode->qid;
        struct vnode *child = vnode_find_child(vnode, element);

        if (!child) {
            if (_expand(mount, element, n - mount_index) != VFS_OK)
                return i;
            else
                continue;
        }

        vnode = child;
        element = pstriter(element);
        i++;
    }

    *node = vnode;

    return n;
}

enum vfs_error vfs_init() {
    errno = 0;
    mq_in = mq_open("vfs/client", O_RDONLY | O_CREAT | O_EXCL);
    if (mq_in == -1) return VFS_NOMEM;

    root = calloc(1, sizeof *root);
    if (!root) return VFS_NOMEM;

    errno = 0;
    root->mq_id = mq_open("vfs/mod/rd", O_WRONLY);
    if (root->mq_id == -1)
        { root = vnode_remove(root); return VFS_NOTFOUND; }

    char uname_buf[16];
    char aname_buf[16];
    pstring *uname = pstrconv(uname_buf, "vfs/client", 16);
    pstring *aname = pstrconv(aname_buf, "/", 16);
    unsigned fid;
    struct vfs_msg vfs_msg;

    /* Setup root */
    fid = fid_count++;
    vfs_msg.fcall.type = Tattach;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = fid;
    vfs_msg.fcall.afid = NOFID;
    vfs_msg.fcall.uname = uname;
    vfs_msg.fcall.aname = aname;
    vfs_msg.mq_id = mq_in;

    if (!vfs_msg_send(&vfs_msg, root->mq_id, mq_in))
        { root = vnode_remove(root); return VFS_NOTFOUND; }

    if (vfs_msg.fcall.type == Rerror)
        { root = vnode_remove(root); return VFS_NOTFOUND; }

    root->qid.type = vfs_msg.fcall.qid.type;
    root->qid.id = id_count++;
    root->qid.version = 0;
    root->fid = fid;
    root->children = NULL;
    root->children_no = 0;

    struct vnode *dev = _add_child(root);
    if (!dev) return VFS_NOTFOUND;

    dev->name = pstrconv(NULL, "dev", 4);
    dev->qid.type = QTDIR;
    dev->qid.id = id_count++;
    dev->qid.version = 0;
    dev->mount_node = root;
    dev->mq_id = root->mq_id;

    struct vnode *uart = _add_child(dev);
    if (!uart) return VFS_NOTFOUND;

    errno = 0;
    uart->mq_id = mq_open("vfs/mod/uart", O_WRONLY);
    if (uart->mq_id == -1)
        { root = vnode_remove(root); return VFS_NOTFOUND; }

    /* Setup uart */
    fid = fid_count++;
    vfs_msg.fcall.type = Tattach;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = fid;
    vfs_msg.fcall.afid = NOFID;
    vfs_msg.fcall.uname = uname;
    vfs_msg.fcall.aname = aname;
    vfs_msg.mq_id = mq_in;

    if (!vfs_msg_send(&vfs_msg, uart->mq_id, mq_in))
        { root = vnode_remove(root); return VFS_NOTFOUND; }

    uart->name = pstrconv(NULL, "uart", 5);
    uart->qid.type = vfs_msg.fcall.qid.type;
    uart->qid.id = id_count++;
    uart->qid.version = 0;
    uart->fid = fid;

    return VFS_OK;
}
