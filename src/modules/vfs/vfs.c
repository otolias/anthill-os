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

static struct vnode* _add_child(struct vnode *vnode);
static enum vfs_error _expand(struct vnode *mount, pstring *elements, unsigned short n);
static struct vnode* _find_child(const struct vnode* vnode, const pstring *name);

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
    char buf[VFS_MAX_MSG_LEN];
    unsigned len;

    struct vfs_msg vfs_msg = {
        .fcall = {
            .type = Twalk,
            .tag = tag_count++,
            .fid = NOFID,
            .newfid = NOFID,
            .nwname = n,
            .wname = elements,
        },
        .mq_id = mq_in,
    };

    len = vfs_msg_put(&vfs_msg, buf);
    if (len == 0) return VFS_MQERR;

    if (mq_send(mount->mq_id, buf, len, 0) == -1)
        return VFS_MQERR;

    if (mq_receive(mq_in, buf, VFS_MAX_MSG_LEN, 0) == -1)
        return VFS_MQERR;

    len = vfs_msg_parse(&vfs_msg, buf);
    if (len == 0 || vfs_msg.fcall.type == Rerror)
        return VFS_MQERR;

    struct vnode *vnode = mount;
    pstring *element = elements;
    for (unsigned short i = 0; i < vfs_msg.fcall.nwqid; i++) {
        struct vnode *child = _find_child(vnode, element);
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
        }

        element = pstriter(element);
        vnode = child;
    }

    return VFS_OK;
}

/*
* Search in children of _vnode_ for vnode with name _name_
*
* On success, returns a pointer to the child.
* On failure, returns a null pointer
*/
static struct vnode* _find_child(const struct vnode* vnode, const pstring *name) {
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

struct vnode* vnode_remove(struct vnode *node) {
    if (!node) return NULL;
    if (node->name) { free(node->name); node->name = NULL; }
    if (node->children) { free(node->children); node->children = NULL; }
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
        if (vnode->mq_id != -1) {
            mount = vnode;
            mount_element = element;
            mount_index = i;
        }

        wqid[i] = vnode->qid;
        struct vnode *child = _find_child(vnode, element);

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

    root->qid.type = 0;
    root->qid.id = id_count++;
    root->qid.version = 0;
    root->children = NULL;
    root->children_no = 0;

    errno = 0;
    root->mq_id = mq_open("vfs/mod/rd", O_WRONLY);
    if (root->mq_id == -1)
        { root = vnode_remove(root); return VFS_NOTFOUND; }

    char uname_buf[16];
    char aname_buf[16];
    pstring *uname = pstrconv(uname_buf, "vfs/client", 16);
    pstring *aname = pstrconv(aname_buf, "/", 16);
    struct vfs_msg vfs_msg = {
        .fcall = {
            .type = Tattach,
            .tag = NOTAG,
            .fid = NOFID,
            .afid = NOFID,
            .uname = uname,
            .aname = aname,
        },
        .mq_id = mq_in,
    };

    char buf[VFS_MAX_MSG_LEN];
    unsigned len;

    len = vfs_msg_put(&vfs_msg, buf);
    if (len == 0)
        { root = vnode_remove(root); return VFS_NOTFOUND; }

    if (mq_send(root->mq_id, buf, len, 0) == -1)
        { root = vnode_remove(root); return VFS_NOTFOUND; }

    if (mq_receive(mq_in, buf, VFS_MAX_MSG_LEN, 0) == -1)
        { root = vnode_remove(root); return VFS_NOTFOUND; }

    len = vfs_msg_parse(&vfs_msg, buf);
    if (len == 0)
        { root = vnode_remove(root); return VFS_NOTFOUND; }

    if (vfs_msg.fcall.type == Rerror)
        { root = vnode_remove(root); return VFS_NOTFOUND; }

    return VFS_OK;
}
