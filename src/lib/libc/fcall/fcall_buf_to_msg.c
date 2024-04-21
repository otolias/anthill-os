#include "fcall.h"

#include <string.h>
#include <sys/types.h>

static unsigned char get_uchar(const char *ptr) {
    return *ptr;
}

static unsigned short get_ushort(const char *ptr) {
    return (unsigned char) ptr[0] | ptr[1] << 8;
}

static unsigned int get_uint(const char *ptr) {
    return (unsigned char) ptr[0] | (unsigned char) ptr[1] << 8 |
        (unsigned char) ptr[2] << 16 | ptr[3] << 24;
}

static unsigned long get_ulong(const char *ptr) {
    return get_uint(ptr) | (unsigned long) get_uint(ptr + 4) << 32;
}

unsigned fcall_buf_to_msg(const char *buf, fcall *fcall) {
    const char *ptr = buf;

    unsigned size = get_uint(ptr);
    ptr += INT_SIZE;

    fcall->type = get_uchar(ptr);
    ptr += CHAR_SIZE;
    fcall->tag = get_ushort(ptr);
    ptr += SHRT_SIZE;

    switch (fcall->type) {
        case Tversion:
        case Rversion:
            fcall->msize = get_uint(ptr);
            ptr += INT_SIZE;
            fcall->version = (char *) ptr;
            ptr += strlen(ptr) + 1;
            break;
        case Tattach:
            fcall->fid = get_uint(ptr);
            ptr += INT_SIZE;
            fcall->afid = get_uint(ptr);
            ptr += INT_SIZE;
            fcall->uname = (char *) ptr;
            ptr += strlen(ptr) + 1;
            fcall->aname = (char *) ptr;
            ptr += strlen(ptr) + 1;
            break;
        case Rattach:
            fcall->qid.type = get_uint(ptr);
            ptr += INT_SIZE;
            fcall->qid.version = get_uint(ptr);
            ptr += INT_SIZE;
            fcall->qid.id = get_ulong(ptr);
            ptr += LONG_SIZE;
            break;
        case Tcreate:
            fcall->fid = get_uint(ptr);
            ptr += INT_SIZE;
            fcall->name = (char *) ptr;
            ptr += strlen(ptr) + 1;
            fcall->perm = get_uint(ptr);
            ptr += INT_SIZE;
            fcall->mode = get_uchar(ptr);
            ptr += CHAR_SIZE;
            break;
        case Rcreate:
            fcall->qid.type = get_uint(ptr);
            ptr += INT_SIZE;
            fcall->qid.version = get_uint(ptr);
            ptr += INT_SIZE;
            fcall->qid.id = get_ulong(ptr);
            ptr += LONG_SIZE;
            fcall->iounit = get_uint(ptr);
            ptr += INT_SIZE;
            break;
        case Rerror:
            fcall->ename = (char *) ptr;
            ptr += strlen(ptr) + 1;
            break;
        default:
            return 0;
    }

    if (size != ptr - buf)
        return 0;

    return size;
}
