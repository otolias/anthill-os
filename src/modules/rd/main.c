#include <errno.h>
#include <fcall.h>
#include <mqueue.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/vfs.h>

#include "rd.h"

mqd_t mq_in = -1, mq_vfs = -1;

/* Errors */
FCALL_ERROR(EINVALID, "Invalid message");
FCALL_ERROR(ENOTFOUND, "Requested File not found");
FCALL_ERROR(ECLIENT, "Client error");

static unsigned short _attach(struct vfs_msg *vfs_msg, char *buf) {
    char vfs_name_buf[16];

    char *vfs_name = pstrtoz(vfs_name_buf, vfs_msg->fcall.uname, 16);
    if (!vfs_name) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &EINVALID;
        return vfs_msg_put(vfs_msg, buf);
    }

    errno = 0;
    mq_vfs = mq_open(vfs_name, O_WRONLY);
    if (mq_vfs == -1) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &EINVALID;
        return vfs_msg_put(vfs_msg, buf);
    }

    const struct header *header = rd_find(vfs_msg->fcall.aname);
    if (!header) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOTFOUND;
        return vfs_msg_put(vfs_msg, buf);
    }

    vfs_msg->fcall.type = Rattach;
    vfs_msg->fcall.qid.type = rd_get_type(header);
    vfs_msg->fcall.qid.version = 0;
    vfs_msg->fcall.qid.id = 0;

    return vfs_msg_put(vfs_msg, buf);
}

/* Handle Twalk message */
static unsigned short _walk(struct vfs_msg *vfs_msg, char *buf) {
    unsigned short total_length = fcall_path_size(&vfs_msg->fcall) + 1; /* Path prefix (/) */

    pstring *path = malloc(total_length + sizeof(*path));
    if (!path) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ECLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

    unsigned short nwqid;
    struct qid wqid[vfs_msg->fcall.nwname];
    char *path_ptr = (char *) path->s;
    pstring *wname_ptr = vfs_msg->fcall.wname;

    *path_ptr++ = '/';
    path->len = 1;

    for (nwqid = 0; nwqid < vfs_msg->fcall.nwname; nwqid++) {
        path->len += wname_ptr->len;
        memcpy(path_ptr, wname_ptr->s, wname_ptr->len);
        path_ptr += wname_ptr->len;

        if (vfs_msg->fcall.nwname - nwqid != 1) {
            *path_ptr++ = '/';
            path->len ++;
        }

        const struct header *header = rd_find(path);
        if (!header) break;

        wqid[nwqid].type = rd_get_type(header);
        wqid[nwqid].version = 0;
        wqid[nwqid].id = 0;

        wname_ptr = pstriter(wname_ptr);
    }

    if (nwqid == 0) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOTFOUND;
    } else {
        vfs_msg->fcall.type = Rwalk;
        vfs_msg->fcall.nwqid = nwqid;
        vfs_msg->fcall.wqid = wqid;
    }

    unsigned result_length = vfs_msg_put(vfs_msg, buf);
    free(path);
    return result_length;
}

/*
* Handle vfs message
*/
static void _handle_message(char *buf) {
    struct vfs_msg vfs_msg;

    unsigned len = vfs_msg_parse(&vfs_msg, buf);
    if (len == 0) {
        puts("MOD_RD::Failed to read message");
        return;
    }

    if (mq_vfs == -1 && vfs_msg.fcall.type != Tattach)
        return;

    switch (vfs_msg.fcall.type) {
        case Tattach:
            if (vfs_msg.fcall.uname->len == 0)
                return;

            len = _attach(&vfs_msg, buf);
            break;

        case Twalk:
            len = _walk(&vfs_msg, buf);
            break;

        default:
            printf("MOD_RD::Unsupported message type %c", vfs_msg.fcall.type);
            break;
    }

    if (len == 0) {
        puts("MOD_RD::Failed to write message");
        return;
    }

    errno = 0;
    if (mq_send(mq_vfs, buf, len, 0) == -1)
        printf("MOD_RD::Failed to send message::%d\n", errno);
}

/*
* Setup in and out mqueues
*/
static bool _init(void) {
    errno = 0;
    mq_in = mq_open("vfs/mod/rd", O_RDONLY | O_CREAT | O_EXCL, 0, 0);
    if (mq_in == -1) {
        printf("MOD_RD::Failed to open input mqueue::%d\n", errno);
        return false;
    }

    return true;
}

/*
* Close and unlink mqueues
*/
static void _deinit(void) {
    if (mq_vfs) {
        errno = 0;
        if (mq_close(mq_vfs) == -1)
            printf("MOD_RD::Failed to close vfs queue::%d\n", errno);
    }

    if (mq_in) {
        errno = 0;
        if (mq_close(mq_in) == -1)
            printf("MOD_RD::Failed to close queue::%d\n", errno);

        errno = 0;
        if (mq_unlink("vfs/mod/rd") == -1)
            printf("MOD_RD::Failed to unlink queue::%d\n", errno);
    }
}

int main(void) {
    char buf[VFS_MAX_MSG_LEN];

    if(!_init())
    { _deinit(); return 1; }

    while (1) {
        errno = 0;
        if (mq_receive(mq_in, buf, VFS_MAX_MSG_LEN, 0) == -1) {
            printf("MOD_RD::Failed to read message::%d\n", errno);
            continue;
        }

        _handle_message(buf);
    }

    _deinit();
}
