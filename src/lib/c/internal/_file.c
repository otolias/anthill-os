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

static bool initialised = false;
static mqd_t mq_in = -1;
static mqd_t mq_vfs = -1;
static char vfs_name[16];
static unsigned fid_count = 2;
static unsigned short tag_count = 0;

static bool _check_init(void) {
    if (!initialised) {
        if (!file_init())
            return false;
    }

    return true;
}

void *file_alloc(FILE *stream) {
    stream->buf = malloc(BUFSIZ);
    if (!stream->buf)
        { errno = ENOMEM; return 0; }

    if (stream->flags & F_READ) {
        stream->r_pos = stream->buf;
        stream->r_end = stream->buf;
    }

    if (stream->flags & F_WRITE) {
        stream->w_pos = stream->buf;
        stream->w_end = stream->buf + BUFSIZ;
    }

    return stream->buf;
}

bool file_close(FILE *stream) {
    char buf[VFS_MAX_MSG_LEN];
    struct vfs_msg vfs_msg;
    vfs_msg.fcall.type = Tclunk;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = stream->fid;
    vfs_msg.mq_id = mq_in;

    bool success = vfs_msg_send(&vfs_msg, buf, mq_vfs, mq_in);

    if (vfs_msg.fcall.type != Rclunk)
        success = false;

    if (stream->buf)
        free(stream->buf);

    stream->fid = NOFID;
    stream->flags = 0;
    stream->offset = 0;
    stream->buf = NULL;
    stream->r_pos = NULL;
    stream->r_end = NULL;
    stream->w_pos = NULL;
    stream->w_end = NULL;

    return success;
}

void file_deinit(void) {
    for (int i = 0; i < FOPEN_MAX; i++) {
        FILE *f = &open_files[i];
        if (f->flags & F_OPEN)
            fclose(f);
    }

    fflush(stdout); fflush(stderr);

    if (mq_in != -1) { mq_close(mq_in); mq_in = 0; mq_unlink(vfs_name); }

    if (mq_vfs != -1) { mq_close(mq_vfs); mq_vfs = 0; }
}

FILE* file_find(unsigned fid) {
    FILE* f = open_files;
    for (int i = 0; i < FOPEN_MAX; i++) {
        if (f[i].fid == fid && f[i].flags & F_OPEN)
            return &f[i];
    }

    return NULL;
}

int file_get_stat(unsigned fid, struct stat *stat) {
    char buf[VFS_MAX_MSG_LEN];
    struct vfs_msg vfs_msg;
    vfs_msg.fcall.type = Tstat;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = fid;
    vfs_msg.mq_id = mq_in;

    if (!vfs_msg_send(&vfs_msg, buf, mq_vfs, mq_in)) {
        errno = EIO;
        return -1;
    }

    if (vfs_msg.fcall.type != Rstat) {
        errno = EBADF;
        return -1;
    }

    stat->st_size = vfs_msg.fcall.stat->length;

    return 0;
}

bool file_init(void) {
    /* This function is called once during the process initialisation. In case it fails
     * because the VFS server has not been initialised or it is called during the VFS
     * server initialisation, it is called with each file_* function */
    pid_t pid = getpid();

    if (snprintf(vfs_name, 16, "vfs/%ld", pid) > 16)
        return false;

    /* Setup vfs queues. */

    if (mq_vfs == -1) {
        mq_vfs = mq_open("vfs/server", O_WRONLY);
        if (mq_vfs == -1) return false;
    }

    if (mq_in == -1) {
        mq_in = mq_open(vfs_name, O_RDONLY | O_CREAT | O_EXCL);
        if (mq_in == -1) return false;
    }

    /* Send Tattach */
    unsigned short tag = tag_count++;
    char uname_buf[16];
    char aname_buf[16];
    pstring *uname = pstrconv(uname_buf, vfs_name, 16);
    pstring *aname = pstrconv(aname_buf, "/", 16);

    char buf[VFS_MAX_MSG_LEN];
    struct vfs_msg vfs_msg;
    vfs_msg.fcall.type = Tattach;
    vfs_msg.fcall.tag = tag;
    vfs_msg.fcall.fid = 0;
    vfs_msg.fcall.afid = NOFID;
    vfs_msg.fcall.uname = uname;
    vfs_msg.fcall.aname = aname;
    vfs_msg.mq_id = mq_in;

    if (vfs_msg_send(&vfs_msg, buf, mq_vfs, mq_in) == 0)
        return false;

    if (vfs_msg.fcall.type != Rattach)
        return false;

    open_files[0].flags |= F_OPEN;
    open_files[0].fid = 0;

    initialised = true;

    return true;
}

int file_mount(const FILE *stream, const char *restrict path) {
    unsigned mfid = fid_count++;
    unsigned len;

    unsigned short nwname;
    pstring *wname;

    nwname = fcall_path_split(&wname, path);
    if (!nwname)
        return NULL;

    if (file_walk(mfid, nwname - 1, wname) != 0)
        return -1;

    char buf[VFS_MAX_MSG_LEN];
    struct vfs_msg vfs_msg;

    pstring *name = wname;
    for (unsigned short i = 0; i < nwname - 1; i++)
        name = pstriter(name);

    vfs_msg.fcall.type = Tcreate;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = mfid;
    vfs_msg.fcall.name = name;
    vfs_msg.fcall.perm = DMDIR | DMTMP;
    vfs_msg.fcall.mode = ORDWR | ORCLOSE;

    len = vfs_msg_send(&vfs_msg, buf, mq_vfs, mq_in);
    free(wname);
    if (len == 0 || vfs_msg.fcall.type != Rcreate) {
        errno = EIO;
        return -1;
    }

    vfs_msg.fcall.type = Tmount;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = stream->fid;
    vfs_msg.fcall.mfid = mfid;

    len = vfs_msg_send(&vfs_msg, buf, mq_vfs, mq_in);
    if (len == 0 || vfs_msg.fcall.type != Rmount) {
        errno = EIO;
        return -1;
    }

    return 0;
}

FILE *file_open(const char *restrict pathname, int oflag) {
    if (!_check_init())
        { errno = EACCES; return NULL; }

    unsigned int fid = fid_count++;
    char buf[VFS_MAX_MSG_LEN];
    struct vfs_msg vfs_msg;

    pstring *wname;
    int nwname = fcall_path_split(&wname, pathname);
    if (nwname == -1)
        { errno = EACCES; return NULL; }

    if (file_walk(fid, nwname, wname) != 0)
        return NULL;

    int file_flags = 0;
    if (oflag & O_RDONLY) {
        vfs_msg.fcall.mode = OREAD;
        file_flags |= F_READ;
    }
    else if (oflag & O_WRONLY) {
        vfs_msg.fcall.mode = OWRITE;
        file_flags |= F_WRITE;
    }
    else {
        vfs_msg.fcall.mode = O_RDWR;
        file_flags |= (F_READ | F_WRITE);
    }

    if (oflag & O_TRUNC) vfs_msg.fcall.mode |= OTRUNC;
    if (oflag & O_APPEND) file_flags |= F_APPEND;
    if (oflag & O_DIRECTORY) file_flags |= F_DIR;

    vfs_msg.fcall.type = Topen;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = fid;
    vfs_msg.mq_id = mq_in;

    if (!vfs_msg_send(&vfs_msg, buf, mq_vfs, mq_in))
        { errno = EACCES; return NULL; }

    if (vfs_msg.fcall.type != Ropen)
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
    f->flags |= file_flags | F_OPEN;
    f->offset = 0;

    return f;
}

unsigned file_read(FILE *stream, unsigned n) {
    if (!_check_init())
        { errno = EIO; return 0; }

    if (!stream->buf) {
        if (!file_alloc(stream))
            return 0;
    }

    size_t buf_fill = stream->r_end - stream->r_pos;

    if (buf_fill > 0 && buf_fill < n) {
        stream->flags |= F_EOF;
        return 0;
    }

    struct vfs_msg vfs_msg;

    unsigned current_buffer_size = 0;

    while (current_buffer_size < n) {
        unsigned remaining = n - current_buffer_size;
        unsigned count = remaining > VFS_MAX_IOUNIT ? VFS_MAX_IOUNIT : remaining;
        char buf[VFS_MAX_MSG_LEN];
        vfs_msg.fcall.type = Tread;
        vfs_msg.fcall.tag = tag_count++;
        vfs_msg.fcall.fid = stream->fid;
        vfs_msg.fcall.count = count;
        vfs_msg.fcall.offset = stream->offset + current_buffer_size;
        vfs_msg.mq_id = mq_in;

        if (!vfs_msg_send(&vfs_msg, buf, mq_vfs, mq_in))
            { errno = EIO; return 0; }

        if (vfs_msg.fcall.type != Rread)
            { errno = EIO; return 0; }

        unsigned read = (vfs_msg.fcall.count < count) ? vfs_msg.fcall.count : count;
        memcpy(stream->buf + current_buffer_size, vfs_msg.fcall.data, read);

        current_buffer_size += read;

        if (vfs_msg.fcall.count < count)
            break;
    }

    stream->r_pos = stream->buf;
    stream->r_end = stream->buf + current_buffer_size;
    return current_buffer_size;
}

int file_walk(unsigned fid, unsigned short nwname, pstring *wname) {
    char buf[VFS_MAX_MSG_LEN];
    struct vfs_msg vfs_msg;
    vfs_msg.fcall.type = Twalk;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = open_files[0].fid;
    vfs_msg.fcall.newfid = fid;
    vfs_msg.fcall.nwname = nwname;
    vfs_msg.fcall.wname = wname;
    vfs_msg.mq_id = mq_in;

    /* TODO: Relative path handling */
    if (vfs_msg_send(&vfs_msg, buf, mq_vfs, mq_in) == 0)
        { errno = EACCES; return -1; }

    if (vfs_msg.fcall.type != Rwalk)
        { errno = EACCES; return -1; }

    return 0;
}

ssize_t file_write(FILE *stream) {
    if (!_check_init())
        { errno = EIO; return 0; }

    if (!(stream->flags & (F_WRITE | F_OPEN))) {
        errno = EBADF;
        return EOF;
    }

    unsigned buf_fill = stream->w_end - stream->w_pos;

    if (!stream->buf || buf_fill == 0)
        return 0;

    char buf[VFS_MAX_MSG_LEN];
    struct vfs_msg vfs_msg;
    unsigned remaining = stream->w_pos - stream->buf;
    unsigned bytes_sent = 0;

    while (remaining > 0) {
        unsigned count = remaining > VFS_MAX_IOUNIT ? VFS_MAX_IOUNIT : remaining;

        vfs_msg.fcall.type = Twrite;
        vfs_msg.fcall.tag = tag_count++;
        vfs_msg.fcall.fid = stream->fid;
        vfs_msg.fcall.offset = stream->offset + bytes_sent;
        vfs_msg.fcall.count = count;
        vfs_msg.fcall.data = (unsigned char *) stream->buf;
        vfs_msg.mq_id = mq_in;

        if (!vfs_msg_send(&vfs_msg, buf, mq_vfs, mq_in))
            { errno = EIO; return bytes_sent; }

        bytes_sent += vfs_msg.fcall.count;
        stream->offset += bytes_sent;

        if (vfs_msg.fcall.count < count)
            { errno = EIO; return bytes_sent; }

        remaining -= vfs_msg.fcall.count;
    }

    if (stream->flags & F_APPEND)
        stream->offset = -1;

    stream->w_pos = stream->buf;
    return bytes_sent;
}
