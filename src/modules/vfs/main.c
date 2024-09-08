/*
* The VFS uses the 9p2000 protocol to communicate with
* processes and the underlying drivers. It acts as a server
* to the system processes and as a client to the filesystem
* drivers.
*
* In addition to the protocol, it expects an mqd_t unsigned integer
* at the tail of the message, to indicate where the reply should go
*
* Files are represented by a tree of vnodes, which hold information
* such as its path, its children and how to access it.
*
* Notes:
* - Currently, no caching is supported or considered.
* - Each vnode holds its driver fid in the id field of the qid structure
*/

#include <errno.h>
#include <fcall.h>
#include <mqueue.h>
#include <pstring.h>
#include <stddef.h>
#include <sys/vfs.h>

#include <vfs/client.h>

#include "vfs.h"
#include "vnode.h"

/* Errors */
FCALL_ERROR(EINVALID, "Invalid message");
FCALL_ERROR(ENOCLIENT, "Failed to find client");
FCALL_ERROR(ESERVER, "Server failed");
FCALL_ERROR(ENOFID, "Unknown fid");
FCALL_ERROR(ENOFILE, "File not found");

static unsigned short _attach(struct vfs_msg *vfs_msg, char *buf) {
    if (vfs_msg->fcall.uname->len == 0)
        return 0;

    char client_name[32];
    if (!pstrtoz(client_name, vfs_msg->fcall.uname, 32))
        return 0;

    struct vfs_client *client = vfs_client_create(client_name);
    if (!client)
        return 0;

    if (vfs_msg->fcall.aname->len == 0 || vfs_msg->fcall.fid == NOFID) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &EINVALID;
        return vfs_msg_put(vfs_msg, buf);
    }

    /* Setup process I/O */
    struct vnode *root = vnode_get_root();
    if (!vfs_client_fid_add(client, vfs_msg->fcall.fid, root)) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOCLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

    /* Only happens on process initialisation */
    if (vfs_msg->fcall.fid == 0) {
        char pstr_buf[16];
        const struct vnode *dev = vnode_find_child(root, pstrconv(pstr_buf, "dev", 16));
        struct vnode *uart = vnode_find_child(dev, pstrconv(pstr_buf, "uart", 16));

        if (!vfs_client_fid_add(client, vfs_msg->fcall.fid + 1, uart)) {
            vfs_msg->fcall.type = Rerror;
            vfs_msg->fcall.ename = &ENOCLIENT;
            return vfs_msg_put(vfs_msg, buf);
        }
    }

    vfs_msg->fcall.type = Rattach;
    vfs_msg->fcall.qid = root->qid;

    return vfs_msg_put(vfs_msg, buf);
}

static unsigned short _clunk(struct vfs_msg *vfs_msg, char *buf) {
    struct vfs_client *client = vfs_client_get(vfs_msg->mq_id);
    if (!client) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOCLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

    if (vfs_msg->fcall.fid == NOFID) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &EINVALID;
        return vfs_msg_put(vfs_msg, buf);
    }

    if (vfs_client_fid_remove(client, vfs_msg->fcall.fid) != 0) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOFID;
        return vfs_msg_put(vfs_msg, buf);
    }

    vfs_msg->fcall.type = Rclunk;

    return vfs_msg_put(vfs_msg, buf);
}

static unsigned short _create(struct vfs_msg *vfs_msg, char *buf) {
    struct vfs_client *client = vfs_client_get(vfs_msg->mq_id);
    if (!client) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOCLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

    if (vfs_msg->fcall.fid == NOFID || !(vfs_msg->fcall.perm & DMDIR)) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &EINVALID;
        return vfs_msg_put(vfs_msg, buf);
    }

    struct vnode *parent = vfs_client_fid_get(client, vfs_msg->fcall.fid);
    if (!parent) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOFID;
        return vfs_msg_put(vfs_msg, buf);
    }

    struct vnode *child = vnode_add_child(parent);
    if (!child) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ESERVER;
        return vfs_msg_put(vfs_msg, buf);
    }

    child->name = pstrdup(NULL, vfs_msg->fcall.name, 0);
    if (!child->name) {
        vnode_remove(child);
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ESERVER;
        return vfs_msg_put(vfs_msg, buf);
    }

    child->qid.type = vfs_msg->fcall.perm;
    child->qid.version = 0;
    child->qid.id = id_count++;
    child->children = NULL;
    child->children_no = 0;
    child->mount_node = NULL;
    child->mq_id = -1;

    vfs_client_fid_update(client, vfs_msg->fcall.fid, child);

    vfs_msg->fcall.type = Rcreate;
    vfs_msg->fcall.qid = child->qid;
    vfs_msg->fcall.iounit = VFS_MAX_IOUNIT;

    return vfs_msg_put(vfs_msg, buf);
}

static unsigned short _mount(struct vfs_msg *vfs_msg, char *buf) {
    struct vfs_client *client = vfs_client_get(vfs_msg->mq_id);
    if (!client) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOCLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

    if (vfs_msg->fcall.fid == NOFID || vfs_msg->fcall.mfid == NOFID) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &EINVALID;
        return vfs_msg_put(vfs_msg, buf);
    }

    const struct vnode *channel = vfs_client_fid_get(client, vfs_msg->fcall.fid);
    struct vnode *mount = vfs_client_fid_get(client, vfs_msg->fcall.mfid);
    if (!channel || !mount) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOFID;
        return vfs_msg_put(vfs_msg, buf);
    }

    if (!vnode_attach(channel, mount)) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ESERVER;
        return vfs_msg_put(vfs_msg, buf);
    }

    vfs_msg->fcall.type = Rmount;
    vfs_msg->fcall.qid = mount->qid;

    return vfs_msg_put(vfs_msg, buf);
}

static unsigned short _open(struct vfs_msg *vfs_msg, char *buf) {
    struct vfs_client *client = vfs_client_get(vfs_msg->mq_id);
    if (!client) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOCLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

    if (vfs_msg->fcall.fid == NOFID) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &EINVALID;
        return vfs_msg_put(vfs_msg, buf);
    }

    const struct vnode *vnode = vfs_client_fid_get(client, vfs_msg->fcall.fid);
    if (!vnode) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOFID;
        return vfs_msg_put(vfs_msg, buf);
    }

    vfs_msg->fcall.type = Ropen;
    vfs_msg->fcall.qid = vnode->qid;
    vfs_msg->fcall.iounit = VFS_MAX_IOUNIT;

    return vfs_msg_put(vfs_msg, buf);
}

static unsigned short _read(struct vfs_msg *vfs_msg, char *buf) {
    struct vfs_client *client = vfs_client_get(vfs_msg->mq_id);
    if (!client) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOCLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

    const struct vnode *vnode = vfs_client_fid_get(client, vfs_msg->fcall.fid);
    if (!vnode) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOFID;
        return vfs_msg_put(vfs_msg, buf);
    }

    if (vnode->mq_id == -1 && !vnode->channel_node) {
        unsigned char read_buf[vfs_msg->fcall.count];
        unsigned count = vnode_read_dir(vnode, read_buf, vfs_msg->fcall.offset,
                                        vfs_msg->fcall.count);

        if (count == 0) {
            vfs_msg->fcall.type = Rerror;
            vfs_msg->fcall.ename = &ESERVER;
            return vfs_msg_put(vfs_msg, buf);
        }

        vfs_msg->fcall.type = Rread;
        vfs_msg->fcall.count = count;
        vfs_msg->fcall.data = read_buf;

        return vfs_msg_put(vfs_msg, buf);
    } else {
        if (vnode_forward(vnode, buf) != VFS_OK) {
            vfs_msg->fcall.type = Rerror;
            vfs_msg->fcall.ename = &ESERVER;
            return vfs_msg_put(vfs_msg, buf);
        }

        return vfs_msg_parse(vfs_msg, buf);
    }
}

static unsigned short _stat(struct vfs_msg *vfs_msg, char *buf) {
    struct vfs_client *client = vfs_client_get(vfs_msg->mq_id);
    if (!client) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOCLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

    const struct vnode *vnode = vfs_client_fid_get(client, vfs_msg->fcall.fid);
    if (!vnode) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOFID;
        return vfs_msg_put(vfs_msg, buf);
    }

    if (vnode_forward(vnode, buf) != VFS_OK) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ESERVER;
        return vfs_msg_put(vfs_msg, buf);
    }

    return vfs_msg_parse(vfs_msg, buf);
}

static unsigned short _walk(struct vfs_msg *vfs_msg, char *buf) {
    struct vfs_client *client = vfs_client_get(vfs_msg->mq_id);
    if (!client) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOCLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

    if (vfs_msg->fcall.fid == NOFID || vfs_msg->fcall.newfid == NOFID) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &EINVALID;
        return vfs_msg_put(vfs_msg, buf);
    }

    struct vnode *node;
    struct qid wqid[vfs_msg->fcall.nwname];
    unsigned short nwqid = vnode_scan(vfs_msg->fcall.wname, vfs_msg->fcall.nwname,
                                      wqid, &node);

    if (node && nwqid == vfs_msg->fcall.nwname) {
        if (!vfs_client_fid_add(client, vfs_msg->fcall.newfid, node)) {
            vfs_msg->fcall.type = Rerror;
            vfs_msg->fcall.ename = &ENOCLIENT;
            return vfs_msg_put(vfs_msg, buf);
        }
    }

    vfs_msg->fcall.type = Rwalk;
    vfs_msg->fcall.nwqid = nwqid;
    vfs_msg->fcall.wqid = wqid;

    return vfs_msg_put(vfs_msg, buf);
}

static unsigned _write(struct vfs_msg *vfs_msg, char *buf) {
    const struct vfs_client *client = vfs_client_get(vfs_msg->mq_id);
    if (!client) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOCLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

    if (vfs_msg->fcall.fid == NOFID) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &EINVALID;
        return vfs_msg_put(vfs_msg, buf);
    }

    const struct vnode *vnode = vfs_client_fid_get(client, vfs_msg->fcall.fid);
    if (!vnode) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOFID;
        return vfs_msg_put(vfs_msg, buf);
    }

    if (vnode_forward(vnode, buf) != VFS_OK) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ESERVER;
        return vfs_msg_put(vfs_msg, buf);
    }

    return vfs_msg_parse(vfs_msg, buf);
}

/*
* Handle vfs message
*/
static void _handle_message(char *buf) {
    struct vfs_msg vfs_msg;
    unsigned len;

    len = vfs_msg_parse(&vfs_msg, buf);
    if (len == 0) return;

    switch (vfs_msg.fcall.type) {
        case Tattach:
            len = _attach(&vfs_msg, buf);
            break;

        case Tclunk:
            len = _clunk(&vfs_msg, buf);
            break;

        case Tcreate:
            len = _create(&vfs_msg, buf);
            break;

        case Tmount:
            len = _mount(&vfs_msg, buf);
            break;

        case Topen:
            len = _open(&vfs_msg, buf);
            break;

        case Tread:
            len = _read(&vfs_msg, buf);
            break;

        case Tstat:
            len = _stat(&vfs_msg, buf);
            break;

        case Twalk:
            len = _walk(&vfs_msg, buf);
            break;

        case Twrite:
            len = _write(&vfs_msg, buf);
            break;

        default:
            return;
    }

    if (len == 0) return;

    mq_send(vfs_msg.mq_id, buf, len, 0);
}

int main(void) {
    char buf[VFS_MAX_MSG_LEN];
    enum vfs_error err;

    errno = 0;
    mqd_t mq_in = mq_open("vfs/server", O_RDWR | O_CREAT | O_EXCL, 0, 0);

    err = vfs_init();
    if (err != VFS_OK) {
        return err;
    }

    while(1) {
        errno = 0;
        if (mq_receive(mq_in, buf, VFS_MAX_MSG_LEN, 0) == -1)
            continue;

        _handle_message(buf);
    }

    mq_close(mq_in);
    mq_unlink("vfs/server");
}
