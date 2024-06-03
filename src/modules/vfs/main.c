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
#include <stdio.h>
#include <sys/vfs.h>

#include "vfs.h"
#include "client.h"

static unsigned short _attach(struct vfs_msg *vfs_msg, char *buf);
static unsigned short _open(struct vfs_msg *vfs_msg, char *buf);
static unsigned short _walk(struct vfs_msg *vfs_msg, char *buf);

/* Errors */
FCALL_ERROR(EINVALID, "Invalid message");
FCALL_ERROR(ENOCLIENT, "Failed to find client");
FCALL_ERROR(ENOFID, "Unknown fid");
FCALL_ERROR(ENOFILE, "File not found");

static unsigned short _attach(struct vfs_msg *vfs_msg, char *buf) {
    struct client *client = client_create(vfs_msg->fcall.uname);
    if (!client) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOCLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

    if (vfs_msg->fcall.aname->len == 0 || vfs_msg->fcall.fid == NOFID) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &EINVALID;
        return vfs_msg_put(vfs_msg, buf);
    }

    if (client_open(client, vfs_msg->fcall.fid, vnode_get_root()) != VFS_OK) {
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

static unsigned short _open(struct vfs_msg *vfs_msg, char *buf) {
    struct client *client = client_get(vfs_msg->mq_id);
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

    const struct vnode *vnode = client_get_node(client, vfs_msg->fcall.fid);
    if (!vnode) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOFID;
        return vfs_msg_put(vfs_msg, buf);
    }

    vfs_msg->fcall.type = Ropen;
    vfs_msg->fcall.qid = vnode->qid;
    vfs_msg->fcall.iounit = 0;

    return vfs_msg_put(vfs_msg, buf);
}

static unsigned short _walk(struct vfs_msg *vfs_msg, char *buf) {
    struct client *client = client_get(vfs_msg->mq_id);
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

    if (node && nwqid == vfs_msg->fcall.nwname)
        if (client_open(client, vfs_msg->fcall.newfid, node) != VFS_OK) {
            vfs_msg->fcall.type = Rerror;
            vfs_msg->fcall.ename = &ENOCLIENT;
            return vfs_msg_put(vfs_msg, buf);
        }

    vfs_msg->fcall.type = Rwalk;
    vfs_msg->fcall.nwqid = nwqid;
    vfs_msg->fcall.wqid = wqid;

    return vfs_msg_put(vfs_msg, buf);
}


/*
* Handle vfs message
*/
static void _handle_message(char *buf) {
    struct vfs_msg vfs_msg;
    unsigned len;

    len = vfs_msg_parse(&vfs_msg, buf);
    if (len == 0) {
        puts("VFS::ERROR::Failed to read message");
        return;
    }

    switch (vfs_msg.fcall.type) {
        case Tattach:
            if (vfs_msg.fcall.uname->len == 0)
                return;

            len = _attach(&vfs_msg, buf);
            break;

        case Topen:
            len = _open(&vfs_msg, buf);
            break;

        case Twalk:
            len = _walk(&vfs_msg, buf);
            break;

        default:
            puts("VFS::ERROR::Unknown message type");
            return;
    }

    if (len == 0) {
        puts("VFS::ERROR::Failed to write message");
        return;
    }

    errno = 0;
    if (mq_send(vfs_msg.mq_id, buf, len, 0) == -1)
        printf("VFS::ERROR::Failed to send message::%d\n", errno);
}

int main(void) {
    char buf[VFS_MAX_MSG_LEN];
    enum vfs_error err;

    errno = 0;
    mqd_t mq_in = mq_open("vfs/server", O_RDONLY | O_CREAT | O_EXCL, 0, 0);
    if (mq_in == -1)
        printf("VFS::ERROR::Failed to open vfs/server::ERRNO::%d\n", errno);

    err = vfs_init();
    if (err != VFS_OK) {
        puts("VFS::ERROR::Failed to initialise VFS");
        return err;
    }

    puts("VFS initialised successfully");

    while(1) {
        errno = 0;
        if (mq_receive(mq_in, buf, VFS_MAX_MSG_LEN, 0) == -1) {
            printf("VFS::ERROR::Failed to read message::%d\n", errno);
            continue;
        }

        _handle_message(buf);
    }

    errno = 0;
    if (mq_close(mq_in) == -1)
        printf("VFS::ERROR::Failed to close queue::%d\n", errno);

    errno = 0;
    if (mq_unlink("vfs/server") == -1)
        printf("VFS::ERROR::Failed to unlink queue::%d\n", errno);
}
