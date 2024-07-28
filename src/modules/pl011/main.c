#include <mqueue.h>
#include <pstring.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/vfs.h>

#include <vfs/client.h>

#include "pl011.h"

mqd_t mq_in = -1;

FCALL_ERROR(EINVALID, "Invalid message");
FCALL_ERROR(ECLIENT, "Client error");

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
    vfs_msg->fcall.qid.id = vfs_msg->fcall.fid;

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
    vfs_msg->fcall.count = pl011_write(vfs_msg->fcall.data, vfs_msg->fcall.count);

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

        case Twrite:
            len = _write(&vfs_msg, buf);
            break;

        default:
            break;
    }

    if (len == 0) return;
    mq_send(vfs_msg.mq_id, buf, len, 0);
}

static void _deinit(void) {
    if (mq_in > -1) {
        mq_close(mq_in);
        mq_unlink("vfs/mod/pl011");
    }
}

static bool _init(void) {
    mq_in = mq_open("vfs/mod/pl011", O_RDONLY | O_CREAT | O_EXCL, 0, 0);
    if (mq_in == -1)
        return false;

    return true;
}

int main(void) {
    char buf[VFS_MAX_MSG_LEN];

    if (pl011_init() != PL011_OK) {
        puts("PL011::ERROR::Failed to initialise");
        return 1;
    }

    if (!_init())
        { _deinit(); return 1; }

    while (1) {
        if (mq_receive(mq_in, buf, VFS_MAX_MSG_LEN, 0) == -1)
            continue;

        _handle_message(buf);
    }
}
