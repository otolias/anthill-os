#include <errno.h>
#include <fcall.h>
#include <mqueue.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/vfs.h>

#include <vfs/client.h>

#include "rd.h"

mqd_t mq_in = -1;

/* Errors */
FCALL_ERROR(EINVALID, "Invalid message");
FCALL_ERROR(ENOTFOUND, "Requested File not found");
FCALL_ERROR(ECLIENT, "Client error");

static unsigned short _attach(struct vfs_msg *vfs_msg, char *buf) {
    if (vfs_msg->fcall.uname->len == 0)
        return 0;

    char client_name[32];
    if (!pstrtoz(client_name, vfs_msg->fcall.uname, 32))
        return 0;

    struct vfs_client *client = vfs_client_create(client_name);
    if (!client)
        return 0;

    if (vfs_msg->fcall.fid == NOFID || vfs_msg->fcall.afid != NOFID) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &EINVALID;
        return vfs_msg_put(vfs_msg, buf);
    }

    struct header *header = rd_find(vfs_msg->fcall.aname);
    if (!header) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOTFOUND;
        return vfs_msg_put(vfs_msg, buf);
    }

    vfs_client_fid_add(client, vfs_msg->fcall.fid, header);

    vfs_msg->fcall.type = Rattach;
    vfs_msg->fcall.qid.type = rd_get_type(header);
    vfs_msg->fcall.qid.version = 0;
    vfs_msg->fcall.qid.id = vfs_msg->fcall.fid;

    return vfs_msg_put(vfs_msg, buf);
}

static unsigned short _read(struct vfs_msg *vfs_msg, char *buf) {
    const struct vfs_client *client = vfs_client_get(vfs_msg->mq_id);
    if (!client) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ECLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

    const struct header *header = vfs_client_fid_get(client, vfs_msg->fcall.fid);
    if (!header) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOTFOUND;
        return vfs_msg_put(vfs_msg, buf);
    }

    unsigned char read_buf[VFS_MAX_IOUNIT];
    unsigned count;

    switch (header->type) {
        case RDT_FILE:
            count = rd_read_file(header, read_buf, vfs_msg->fcall.offset,
                                 vfs_msg->fcall.count);
            break;

        case RDT_DIR:
            count = rd_read_dir(header, read_buf, vfs_msg->fcall.offset,
                                vfs_msg->fcall.count, VFS_MAX_IOUNIT);
            break;

        default:
            vfs_msg->fcall.type = Rerror;
            vfs_msg->fcall.ename = &EINVALID;
            return vfs_msg_put(vfs_msg, buf);
    }

    vfs_msg->fcall.type = Rread;
    vfs_msg->fcall.count = count;
    vfs_msg->fcall.data = (unsigned char *) read_buf;
    return vfs_msg_put(vfs_msg, buf);
}

static unsigned short _stat(struct vfs_msg *vfs_msg, char *buf) {
    const struct vfs_client *client = vfs_client_get(vfs_msg->mq_id);
    if (!client) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ECLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

    const struct header *header = vfs_client_fid_get(client, vfs_msg->fcall.fid);
    if (!header) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOTFOUND;
        return vfs_msg_put(vfs_msg, buf);
    }

    vfs_msg->fcall.type = Rstat;
    unsigned char stat_buf[VFS_MAX_IOUNIT];
    vfs_msg->fcall.stat = (struct fcall_stat *) stat_buf;
    vfs_msg->fcall.nstat = rd_get_stat(header, vfs_msg->fcall.stat, VFS_MAX_IOUNIT);
    if (vfs_msg->fcall.nstat == 0) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOTFOUND;
        return vfs_msg_put(vfs_msg, buf);
    }

    return vfs_msg_put(vfs_msg, buf);
}

static unsigned short _walk(struct vfs_msg *vfs_msg, char *buf) {
    unsigned short total_length = fcall_path_size(&vfs_msg->fcall) + 1; /* Path prefix (/) */

    if (vfs_msg->fcall.newfid == NOFID) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &EINVALID;
        return vfs_msg_put(vfs_msg, buf);
    }

    struct vfs_client *client = vfs_client_get(vfs_msg->mq_id);
    if (!client) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ECLIENT;
        return vfs_msg_put(vfs_msg, buf);
    }

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

    struct header *header;
    for (nwqid = 0; nwqid < vfs_msg->fcall.nwname; nwqid++) {
        path->len += wname_ptr->len;
        memcpy(path_ptr, wname_ptr->s, wname_ptr->len);
        path_ptr += wname_ptr->len;

        if (vfs_msg->fcall.nwname - nwqid != 1) {
            *path_ptr++ = '/';
            path->len ++;
        }

        header = rd_find(path);
        if (!header) break;

        wqid[nwqid].type = rd_get_type(header);
        wqid[nwqid].version = 0;
        wqid[nwqid].id = NOFID;

        wname_ptr = pstriter(wname_ptr);
    }

    if (nwqid == 0) {
        vfs_msg->fcall.type = Rerror;
        vfs_msg->fcall.ename = &ENOTFOUND;
    } else {
        if (nwqid == vfs_msg->fcall.nwname) {
            vfs_client_fid_add(client, vfs_msg->fcall.newfid, header);
            wqid[nwqid - 1].id = vfs_msg->fcall.newfid;
        }

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

    switch (vfs_msg.fcall.type) {
        case Tattach:
            len = _attach(&vfs_msg, buf);
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

        default:
            printf("MOD_RD::Unsupported message type %c", vfs_msg.fcall.type);
            break;
    }

    if (len == 0) {
        puts("MOD_RD::Failed to write message");
        return;
    }

    errno = 0;
    if (mq_send(vfs_msg.mq_id, buf, len, 0) == -1)
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
