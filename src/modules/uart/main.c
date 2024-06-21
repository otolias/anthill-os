#include <errno.h>
#include <mqueue.h>
#include <pstring.h>
#include <stdbool.h>
#include <sys/vfs.h>

#include <vfs/client.h>

#include "uart.h"

mqd_t mq_in = -1;

FCALL_ERROR(ECLIENT, "Client error");
FCALL_ERROR(EINVALID, "Invalid message");
FCALL_ERROR(EUNKNOWN, "Unknown type");

static unsigned short _attach(struct vfs_msg *vfs_msg, char *buf) {
    if (vfs_msg->fcall.uname->len == 0)
        return 0;

    char client_name[32];
    if (!pstrtoz(client_name, vfs_msg->fcall.uname, 32))
        return 0;

    const struct vfs_client *client = vfs_client_create(client_name);
    if (!client)
        return 0;

    if (vfs_msg->fcall.fid == NOFID || vfs_msg->fcall.afid != NOFID) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &EINVALID;
        return vfs_msg_put(vfs_msg, buf);
    }

    vfs_msg->fcall.type = Rattach;
    vfs_msg->fcall.qid.type = QTFILE;
    vfs_msg->fcall.qid.version = 0;
    vfs_msg->fcall.qid.id = 0;

    return vfs_msg_put(vfs_msg, buf);
}

static unsigned short _read(struct vfs_msg *vfs_msg, char *buf) {
    const struct vfs_client *client = vfs_client_get(vfs_msg->mq_id);
    if (!client) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ECLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

    unsigned char read_buf[vfs_msg->fcall.count];
    unsigned count = uart_read(read_buf, vfs_msg->fcall.count);

    vfs_msg->fcall.type = Rread;
    vfs_msg->fcall.count = count;
    vfs_msg->fcall.data = (unsigned char *) read_buf;
    return vfs_msg_put(vfs_msg, buf);
}

static unsigned short _write(struct vfs_msg *vfs_msg, char *buf) {
    const struct vfs_client *client = vfs_client_get(vfs_msg->mq_id);
    if (!client) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ECLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

    vfs_msg->fcall.type = Rwrite;
    vfs_msg->fcall.count = uart_write(vfs_msg->fcall.data, vfs_msg->fcall.count);

    return vfs_msg_put(vfs_msg, buf);
}

static void _handle_message(char *buf) {
    struct vfs_msg vfs_msg;

    unsigned len = vfs_msg_parse(&vfs_msg, buf);
    if (len == 0) return;

    switch (vfs_msg.fcall.type) {
        case Tattach:
            len = _attach(&vfs_msg, buf);
            break;

        case Tread:
            len = _read(&vfs_msg, buf);
            break;

        case Twrite:
            len = _write(&vfs_msg, buf);
            break;

        default:
            vfs_msg.fcall.type = Rerror;
            vfs_msg.fcall.ename = &EUNKNOWN;
            len = vfs_msg_put(&vfs_msg, buf);
    }

    mq_send(vfs_msg.mq_id, buf, len, 0);
}

static bool _init(void) {
    errno = 0;
    mq_in = mq_open("vfs/mod/uart", O_RDONLY | O_CREAT | O_EXCL, 0, 0);
    if (mq_in == -1)
        return false;

    return true;
}

static void _deinit(void) {
    if (mq_in > -1) {
        mq_close(mq_in);
        mq_unlink("vfs/mod/uart");
    }
}

int main(void) {
    char buf[VFS_MAX_MSG_LEN];

    if (!_init())
        { _deinit(); return 1; }

    while (1) {
        errno = 0;
        if (mq_receive(mq_in, buf, VFS_MAX_MSG_LEN, 0) == -1) {
            continue;
        }

        _handle_message(buf);
    }
}
