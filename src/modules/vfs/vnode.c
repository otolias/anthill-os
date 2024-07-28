#include "vnode.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "vfs.h"

struct vnode _root_vnode;

/*
* Generate a Twalk message for the driver
*/
static enum vfs_error _expand(struct vnode *mount, pstring *elements, unsigned short n) {
    unsigned new_fid = fid_count++;

    char buf[VFS_MAX_MSG_LEN];
    struct vfs_msg vfs_msg;
    vfs_msg.fcall.type = Twalk;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = mount->fid;
    vfs_msg.fcall.newfid = new_fid;
    vfs_msg.fcall.nwname = n;
    vfs_msg.fcall.wname = elements;
    vfs_msg.mq_id = vfs_in;

    if (!vfs_msg_send(&vfs_msg, buf, mount->mq_id, vfs_in))
        return VFS_MQERR;

    if (vfs_msg.fcall.type != Rwalk)
        return VFS_MQERR;

    struct vnode *vnode = mount;
    pstring *element = elements;
    for (unsigned short i = 0; i < vfs_msg.fcall.nwqid; i++) {
        struct vnode *child = vnode_find_child(vnode, element);
        if (!child) {
            child = vnode_add_child(vnode);
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
            child->mount_node = mount;
            child->mq_id = vnode->mq_id;
        }

        element = pstriter(element);
        vnode = child;
    }

    vnode->fid = new_fid;

    return VFS_OK;
}

struct vnode* vnode_add_child(struct vnode *vnode) {
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
    return &_root_vnode;
}

enum vfs_error vnode_forward(const struct vnode *vnode, char *buf) {
    unsigned msg_size;

    /* Keep old tag and mq_id */
    unsigned short old_tag = *((unsigned short *) (buf + FCALL_TAG_OFF));
    msg_size = (unsigned) *(buf + FCALL_SIZE_OFF);
    mqd_t old_mq = *((mqd_t *) (buf + msg_size));

    /* Replace tag, fid and mq_id */
    *(buf + FCALL_TAG_OFF) = tag_count++;
    *(buf + FCALL_FID_OFF) = vnode->fid;
    *(buf + msg_size) = vfs_in;

    if (mq_send(vnode->mq_id, buf, msg_size + sizeof(mqd_t), 0) == -1)
        return VFS_MQERR;

    if (mq_receive(vfs_in, buf, VFS_MAX_MSG_LEN, 0) == -1)
        return VFS_MQERR;

    /* Restore tag and mq_id */
    *(buf + FCALL_TAG_OFF) = old_tag;
    msg_size = (unsigned) *(buf + FCALL_SIZE_OFF);
    *(buf + msg_size) = old_mq;

    return VFS_OK;
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
    struct vnode *mount = vnode_get_root();
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
            if (_expand(mount, mount_element, n - mount_index) != VFS_OK)
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
