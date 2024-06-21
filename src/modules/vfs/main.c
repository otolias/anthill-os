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
* Currently, no caching is supported or considered.
*/

#include <errno.h>
#include <fcall.h>
#include <mqueue.h>
#include <pstring.h>
#include <sys/vfs.h>

#include <vfs/client.h>

#include "vfs.h"

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
    char pstr_buf[16];
    struct vnode *root = vnode_get_root();
    const struct vnode *dev = vnode_find_child(root, pstrconv(pstr_buf, "dev", 16));
    struct vnode *uart = vnode_find_child(dev, pstrconv(pstr_buf, "uart", 16));

    if (!vfs_client_add_fid(client, vfs_msg->fcall.fid, uart) ||
        !vfs_client_add_fid(client, vfs_msg->fcall.fid + 1, uart) ||
        !vfs_client_add_fid(client, vfs_msg->fcall.fid + 2, uart) ||
        !vfs_client_add_fid(client, vfs_msg->fcall.fid + 3, root)) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOCLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

    vfs_msg->fcall.type = Rattach;
    vfs_msg->fcall.qid.type = 0;
    vfs_msg->fcall.qid.version = 0;
    vfs_msg->fcall.qid.id = 0;

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

    if (vfs_client_remove_fid(client, vfs_msg->fcall.fid) != 0) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOFID;
        return vfs_msg_put(vfs_msg, buf);
    }

    vfs_msg->fcall.type = Rclunk;

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

    const struct vnode *vnode = vfs_client_get_fid(client, vfs_msg->fcall.fid);
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

    const struct vnode *vnode = vfs_client_get_fid(client, vfs_msg->fcall.fid);
    if (!vnode) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOFID;
        return vfs_msg_put(vfs_msg, buf);
    }

    char read_buf[VFS_MAX_MSG_LEN];
    struct vfs_msg read_msg;
    read_msg.fcall.type = Tread;
    read_msg.fcall.offset = vfs_msg->fcall.offset;
    read_msg.fcall.count = vfs_msg->fcall.count;

    if (vnode_forward(vnode, &read_msg, read_buf) == 0) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ESERVER;
        return vfs_msg_put(vfs_msg, buf);
    }

    if (read_msg.fcall.type != Rread) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ESERVER;
        return vfs_msg_put(vfs_msg, buf);
    }

    vfs_msg->fcall.type = Rread;
    vfs_msg->fcall.count = read_msg.fcall.count;
    vfs_msg->fcall.data = read_msg.fcall.data;
    return vfs_msg_put(vfs_msg, buf);
}

static unsigned short _stat(struct vfs_msg *vfs_msg, char *buf) {
    struct vfs_client *client = vfs_client_get(vfs_msg->mq_id);
    if (!client) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOCLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

    const struct vnode *vnode = vfs_client_get_fid(client, vfs_msg->fcall.fid);
    if (!vnode) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOFID;
        return vfs_msg_put(vfs_msg, buf);
    }

    char stat_buf[VFS_MAX_MSG_LEN];
    struct vfs_msg stat_msg;
    stat_msg.fcall.type = Tstat;

    if (vnode_forward(vnode, &stat_msg, stat_buf) == 0 ||
        stat_msg.fcall.type != Rstat) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ESERVER;
        return vfs_msg_put(vfs_msg, buf);
    }

    vfs_msg->fcall.type = Rstat;
    vfs_msg->fcall.nstat = stat_msg.fcall.nstat;
    vfs_msg->fcall.stat = stat_msg.fcall.stat;
    return vfs_msg_put(vfs_msg, buf);
}

static unsigned short _walk(struct vfs_msg *vfs_msg, char *buf) {
    struct vfs_client *client = vfs_client_get(vfs_msg->mq_id);
    if (!client) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOCLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

    if (vfs_msg->fcall.fid == NOFID || vfs_msg->fcall.newfid == NOFID ||
            vfs_msg->fcall.nwname == 0) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &EINVALID;
        return vfs_msg_put(vfs_msg, buf);
    }

    struct vnode *node;
    struct qid wqid[vfs_msg->fcall.nwname];
    unsigned short nwqid = vnode_scan(vfs_msg->fcall.wname, vfs_msg->fcall.nwname,
                                      wqid, &node);
    if (nwqid == 0) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOFILE;
        return vfs_msg_put(vfs_msg, buf);
    }

    if (node && nwqid == vfs_msg->fcall.nwname) {
        if (!vfs_client_add_fid(client, vfs_msg->fcall.newfid, node)) {
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

    const struct vnode *vnode = vfs_client_get_fid(client, vfs_msg->fcall.fid);
    if (!vnode) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOFID;
        return vfs_msg_put(vfs_msg, buf);
    }

    char write_buf[VFS_MAX_MSG_LEN];
    struct vfs_msg write_msg;
    write_msg.fcall.type = Twrite;
    write_msg.fcall.offset = vfs_msg->fcall.offset;
    write_msg.fcall.count = vfs_msg->fcall.count;
    write_msg.fcall.data = vfs_msg->fcall.data;

    if (vnode_forward(vnode, &write_msg, write_buf) == 0) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ESERVER;
        return vfs_msg_put(vfs_msg, buf);
    }

    if (write_msg.fcall.type != Rwrite) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ESERVER;
        return vfs_msg_put(vfs_msg, buf);
    }

    vfs_msg->fcall.type = Rwrite;
    vfs_msg->fcall.count = write_msg.fcall.count;
    return vfs_msg_put(vfs_msg, buf);
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
