#include "fcall.h"

#include <string.h>

static unsigned char _get_uchar(const char *ptr) {
    return *ptr;
}

static unsigned short _get_ushort(const char *ptr) {
    return (unsigned char) ptr[0] | ptr[1] << 8;
}

static unsigned int _get_uint(const char *ptr) {
    return (unsigned char) ptr[0] | (unsigned char) ptr[1] << 8 |
        (unsigned char) ptr[2] << 16 | ptr[3] << 24;
}

static unsigned long _get_ulong(const char *ptr) {
    return _get_uint(ptr) | (unsigned long) _get_uint(ptr + 4) << 32;
}

static char* _get_string(char *ptr, char **str) {
    unsigned short length = _get_ushort(ptr);

    /* Make space for terminating byte */
    memmove(ptr, ptr + SHRT_SIZE, length);
    ptr[length] = 0;

    *str = ptr;
    ptr += length + SHRT_SIZE;

    return ptr;
}

unsigned fcall_buf_to_msg(char *buf, fcall *fcall) {
    char *ptr = buf;

    unsigned size = _get_uint(ptr);
    ptr += INT_SIZE;

    fcall->type = _get_uchar(ptr);
    ptr += CHAR_SIZE;
    fcall->tag = _get_ushort(ptr);
    ptr += SHRT_SIZE;

    switch (fcall->type) {
        case Tversion:
        case Rversion:
            fcall->msize = _get_uint(ptr);
            ptr += INT_SIZE;
            ptr = _get_string(ptr, &fcall->version);
            break;

        case Tattach:
            fcall->fid = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->afid = _get_uint(ptr);
            ptr += INT_SIZE;
            ptr = _get_string(ptr, &fcall->uname);
            ptr = _get_string(ptr, &fcall->aname);
            break;

        case Rattach:
            fcall->qid.type = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->qid.version = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->qid.id = _get_ulong(ptr);
            ptr += LONG_SIZE;
            break;

        case Topen:
            fcall->fid = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->mode = _get_uchar(ptr);
            ptr += CHAR_SIZE;
            break;

        case Ropen:
        case Rcreate:
            fcall->qid.type = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->qid.version = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->qid.id = _get_ulong(ptr);
            ptr += LONG_SIZE;
            fcall->iounit = _get_uint(ptr);
            ptr += INT_SIZE;
            break;

        case Tcreate:
            fcall->fid = _get_uint(ptr);
            ptr += INT_SIZE;
            ptr = _get_string(ptr, &fcall->name);
            fcall->perm = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->mode = _get_uchar(ptr);
            ptr += CHAR_SIZE;
            break;

        case Rerror:
            ptr = _get_string(ptr, &fcall->ename);
            break;

        default:
            return 0;
    }

    if (size != ptr - buf)
        return 0;

    return size;
}
