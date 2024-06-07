#include "internal/stdio.h"

#include <errno.h>
#include <fcall.h>
#include <mqueue.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/vfs.h>
#include <unistd.h>

FILE open_files[FOPEN_MAX];

static mqd_t mq_in = -1;
static mqd_t mq_vfs = -1;
static char vfs_name[16];
static unsigned fid_count = 0;
static unsigned short tag_count = 0;

static bool _init(void) {
    pid_t pid = getpid();

    if (snprintf(vfs_name, 16, "vfs/%ld", pid) > 16)
        return false;

    /* Setup vfs queues */

    mq_vfs = mq_open("vfs/server", O_WRONLY);
    if (mq_vfs == -1)
        { errno = EACCES; return false; }

    mq_in = mq_open(vfs_name, O_RDONLY | O_CREAT | O_EXCL);
    if (mq_in == -1)
        { errno = EACCES; return false; }

    /* Send Tattach */

    unsigned len, fid = fid_count++;
    unsigned short tag = tag_count++;
    char buf[128];

    char uname_buf[16];
    char aname_buf[16];
    pstring *uname = pstrconv(uname_buf, vfs_name, 16);
    pstring *aname = pstrconv(aname_buf, "/", 16);
    if (!uname || !aname)
        { errno = EACCES; return false; }

    struct vfs_msg vfs_msg;
    vfs_msg.fcall.type = Tattach;
    vfs_msg.fcall.tag = tag;
    vfs_msg.fcall.fid = fid;
    vfs_msg.fcall.afid = NOFID;
    vfs_msg.fcall.uname = uname;
    vfs_msg.fcall.aname = aname;
    vfs_msg.mq_id = mq_in;

    len = vfs_msg_put(&vfs_msg, buf);
    if (len == 0)
        { errno = EACCES; return false; }

    if (mq_send(mq_vfs, buf, len, 0) == -1)
        { errno = EACCES; return false; }

    if (mq_receive(mq_in, buf, VFS_MAX_MSG_LEN, 0) == -1)
        { errno = EACCES; return false; }

    len = vfs_msg_parse(&vfs_msg, buf);
    if (len == 0 || vfs_msg.fcall.type == Rerror)
        { errno = EACCES; return false; }

    // TODO: Qid?
    open_files[0].fid = fid;

    return true;
}

static void _deinit() {
    if (mq_in != -1) { mq_close(mq_in); mq_in = 0; mq_unlink(vfs_name); }

    if (mq_vfs != -1) { mq_close(mq_vfs); mq_vfs = 0; }
}

static bool _send_msg(struct vfs_msg *msg) {
    char buf[VFS_MAX_MSG_LEN];
    unsigned len;

    len = vfs_msg_put(msg, buf);
    if (len == 0) return false;

    if (mq_send(mq_vfs, buf, len, 0) == -1)
        return false;

    if (mq_receive(mq_in, buf, VFS_MAX_MSG_LEN, 0) == -1)
        return false;

    len = vfs_msg_parse(msg, buf);
    if (len == 0 || msg->fcall.type == Rerror)
        return false;

    return true;
}

FILE *file_open(const char *restrict pathname, int oflag) {
    if (mq_vfs == -1)
        if (!_init()) {
            _deinit();
            return NULL;
        }

    unsigned int fid = fid_count++;
    struct vfs_msg vfs_msg;
    vfs_msg.fcall.type = Twalk;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = open_files[0].fid;
    vfs_msg.fcall.newfid = fid;
    vfs_msg.mq_id = mq_in;

    if (fcall_path_split(&vfs_msg.fcall, pathname) == 0)
        { errno = EACCES; return NULL; }

    /* TODO: Relative path handling */
    if (!_send_msg(&vfs_msg))
        { errno = EACCES; free(vfs_msg.fcall.wname); return NULL; }

    free(vfs_msg.fcall.wname);

    if (oflag & O_RDONLY) vfs_msg.fcall.mode = OREAD;
    else if (oflag & O_WRONLY) vfs_msg.fcall.mode = OWRITE;
    else vfs_msg.fcall.mode = O_RDWR;

    if (oflag & O_TRUNC) vfs_msg.fcall.mode |= OTRUNC;

    vfs_msg.fcall.type = Topen;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = fid;
    vfs_msg.mq_id = mq_in;

    if (!_send_msg(&vfs_msg))
        { errno = EACCES; return NULL; }

    FILE *f = NULL;
    for (unsigned i = 0; i < FOPEN_MAX; i++) {
        if (!(open_files[i].flags & F_OPEN)){
            f = &open_files[i];
            break;
        }
    }

    if (!f)
        { errno = EMFILE; return NULL; }

    f->fid = fid;
    f->flags |= 1 << F_OPEN;
    f->seek_offset = 0;
    f->chunk_index = -1;

    return f;
}

unsigned file_read(FILE *stream) {
    if (!stream->buf) {
        stream->buf = malloc(BUFSIZ);
        if (!stream->buf)
            { errno = ENOMEM; return 0; }

        stream->buf_end = stream->buf + BUFSIZ;
    }

    if (stream->buf_end - stream->buf < BUFSIZ) {
        stream->flags |= 1 << F_EOF;
        return 0;
    }

    char buf[VFS_MAX_MSG_LEN];
    struct vfs_msg vfs_msg;

    unsigned current_buffer_size = 0;

    while (current_buffer_size < BUFSIZ) {
        unsigned remaining = BUFSIZ - current_buffer_size;
        unsigned count = remaining > VFS_MAX_IOUNIT ? VFS_MAX_IOUNIT : remaining;
        unsigned len;
        vfs_msg.fcall.type = Tread;
        vfs_msg.fcall.tag = tag_count++;
        vfs_msg.fcall.fid = stream->fid;
        vfs_msg.fcall.count = count;
        vfs_msg.fcall.offset = stream->seek_offset + current_buffer_size;
        vfs_msg.mq_id = mq_in;

        len = vfs_msg_put(&vfs_msg, buf);
        if (len == 0)
            { errno = EIO; return 0; }

        if (mq_send(mq_vfs, buf, len, 0) == -1)
            { errno = EIO; return 0; }

        if (mq_receive(mq_in, buf, VFS_MAX_MSG_LEN, 0) == -1)
            { errno = EIO; return 0; }

        len = vfs_msg_parse(&vfs_msg, buf);
        if (len == 0 || vfs_msg.fcall.type == Rerror)
            { errno = EIO; return 0; }

        unsigned n = (vfs_msg.fcall.count < count) ? vfs_msg.fcall.count : count;
        memcpy(stream->buf + current_buffer_size, vfs_msg.fcall.data, n);

        current_buffer_size += n;

        if (vfs_msg.fcall.count < count)
            break;
    }

    stream->buf_end = stream->buf + current_buffer_size;
    stream->chunk_index++;
    return current_buffer_size;
}
