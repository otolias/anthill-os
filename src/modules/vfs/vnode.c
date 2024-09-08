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
    unsigned new_fid = id_count++;

    unsigned char buf[VFS_MAX_MSG_LEN];
    struct vfs_msg vfs_msg;
    vfs_msg.fcall.type = Twalk;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = mount->qid.id;
    vfs_msg.fcall.newfid = new_fid;
    vfs_msg.fcall.nwname = n;
    vfs_msg.fcall.wname = elements;
    vfs_msg.mq_id = vfs_in;

    if (mount->channel_node) {
        unsigned len = fcall_msg_to_buf(&vfs_msg.fcall, buf, VFS_MAX_MSG_LEN);
        if (len == 0)
            return VFS_MQERR;

        if (vnode_wrap(mount, buf, len) != VFS_OK)
            return VFS_MQERR;

        if (fcall_buf_to_msg(buf, &vfs_msg.fcall) == 0)
            return VFS_MQERR;
    } else {
        if (!vfs_msg_send(&vfs_msg, (char *) buf, mount->mq_id, vfs_in))
            return VFS_MQERR;
    }

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

            child->qid = vfs_msg.fcall.wqid[i];
            child->children = NULL;
            child->children_no = 0;
            child->mount_node = mount;
            child->channel_node = mount->channel_node;
            child->mq_id = vnode->mq_id;
        }

        element = pstriter(element);
        vnode = child;
    }

    vnode->qid.id = new_fid;

    return VFS_OK;
}

struct vnode* vnode_add_child(struct vnode *vnode) {
    /* Check if more space is needed */
    unsigned short n = 1;
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

struct vnode* vnode_attach(const struct vnode *channel, struct vnode *mount) {
    mount->channel_node = (struct vnode *) channel;

    unsigned char buf[VFS_MAX_MSG_LEN];

    fcall attach;
    char uname_buf[16];
    char aname_buf[16];
    pstring *uname = pstrconv(uname_buf, "vfs/export", 16);
    pstring *aname = pstrconv(aname_buf, "/", 16);
    attach.type = Tattach;
    attach.tag = tag_count++;
    attach.fid = mount->qid.id;
    attach.afid = NOFID;
    attach.uname = uname;
    attach.aname = aname;

    unsigned attach_len = fcall_msg_to_buf(&attach, buf, VFS_MAX_MSG_LEN);
    if (attach_len == 0)
        return NULL;

    if (vnode_wrap(mount, buf, attach_len) != VFS_OK)
        return NULL;

    if (fcall_buf_to_msg(buf, &attach) == 0)
        return NULL;

    if (attach.type != Rattach)
        return NULL;

    mount->mount_node = NULL;

    return mount;
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

    /* Replace tag and fid */
    *(buf + FCALL_TAG_OFF) = tag_count++;
    *(buf + FCALL_FID_OFF) = vnode->qid.id;

    if (vnode->channel_node) {
        if (vnode_wrap(vnode, (unsigned char *) buf, msg_size) != VFS_OK)
            return VFS_MQERR;
    } else {
        /* Replace mq_id */
        *(buf + msg_size) = vfs_in;

        if (mq_send(vnode->mq_id, buf, msg_size + sizeof(mqd_t), 0) == -1)
            return VFS_MQERR;

        if (mq_receive(vfs_in, buf, VFS_MAX_MSG_LEN, 0) == -1)
            return VFS_MQERR;
    }

    /* Restore tag and mq_id */
    *(buf + FCALL_TAG_OFF) = old_tag;
    msg_size = (unsigned) *(buf + FCALL_SIZE_OFF);
    *(buf + msg_size) = old_mq;

    return VFS_OK;
}

unsigned vnode_read_dir(const struct vnode *vnode, unsigned char *buf,
                        unsigned long offset, unsigned count) {
    unsigned char *buf_pos = buf;

    for (unsigned i = offset; i < vnode->children_no; i++) {
        const struct vnode *child = &vnode->children[i];

        size_t stat_len = sizeof(vnode->qid) + sizeof(unsigned long) +
            pstrlen(child->name);

        if (buf_pos - buf + stat_len > count) break;

        memcpy(buf_pos, &child->qid, sizeof(child->qid));
        buf_pos += sizeof(child->qid);

        memset(buf_pos, 0, sizeof(unsigned long));
        buf_pos += sizeof(unsigned long);

        pstrdup(buf_pos, child->name, pstrlen(child->name));
        buf_pos += pstrlen(child->name);
    }

    return buf_pos - buf;
}

struct vnode* vnode_remove(struct vnode *node) {
    if (!node) return NULL;

    if (node->children) {
        for (unsigned i = 0; i < node->children_no; i++)
            node->children = vnode_remove(&node->children[i]);

        free(node->children); node->children = NULL;
    }

    if (node->name) { free(node->name); node->name = NULL; }

    if (node != &_root_vnode)
        free(node);

    return NULL;
}

unsigned short vnode_scan(pstring *elements, unsigned short n, struct qid *wqid,
                          struct vnode **node) {
    struct vnode *mount;
    struct vnode *vnode = vnode_get_root();
    pstring *mount_element;
    pstring *element = elements;
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

    /* Establish fid if it doesn't have one */
    if (vnode->qid.id == NOFID) {
        _expand(mount, mount_element, n - mount_index);
    }

    return n;
}

enum vfs_error vnode_wrap(const struct vnode *vnode, unsigned char *buf, size_t n) {
    /* Replace fid */
    *(buf + FCALL_FID_OFF) = vnode->qid.id;

    char rw_buf[VFS_MAX_MSG_LEN];
    struct vfs_msg vfs_msg;
    vfs_msg.fcall.type = Twrite;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = vnode->channel_node->qid.id;
    vfs_msg.fcall.offset = 0;
    vfs_msg.fcall.count = n;
    vfs_msg.fcall.data = buf;
    vfs_msg.mq_id = vfs_in;

    if (!vfs_msg_send(&vfs_msg, rw_buf, vnode->channel_node->mq_id, vfs_in))
        return VFS_MQERR;

    if (vfs_msg.fcall.type != Rwrite)
        return VFS_MQERR;

    unsigned char *pos = buf;

    /* Retry until there is no more data to be read */
    do {
        vfs_msg.fcall.type = Tread;
        vfs_msg.fcall.tag = tag_count++;
        vfs_msg.fcall.fid = vnode->mount_node->qid.id;
        vfs_msg.fcall.offset = 0;
        vfs_msg.fcall.count = VFS_MAX_IOUNIT;
        vfs_msg.mq_id = vfs_in;

        if (vfs_msg_send(&vfs_msg, rw_buf, vnode->channel_node->mq_id, vfs_in) == 0)
            return VFS_MQERR;

        if (vfs_msg.fcall.type != Rread)
            return VFS_MQERR;

        memcpy(pos, vfs_msg.fcall.data, vfs_msg.fcall.count);
        pos += vfs_msg.fcall.count;
    } while (vfs_msg.fcall.count != 0);

    return VFS_OK;
}
