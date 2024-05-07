#include "fcall.h"

#include <string.h>
#include <sys/types.h>

static unsigned _put_uchar(char *ptr, unsigned short val) {
    ptr[0] = val;
    return CHAR_SIZE;
}

static unsigned _put_ushort(char *ptr, unsigned short val) {
    ptr[0] = val; ptr[1] = val >> 8;
    return SHRT_SIZE;
}

static unsigned _put_uint(char *ptr, unsigned int val) {
    ptr[0] = val; ptr[1] = val >> 8; ptr[2] = val >> 16; ptr[3] = val >> 24;
    return INT_SIZE;
}

static unsigned _put_ulong(char *ptr, unsigned long val) {
    ptr[0] = val; ptr[1] = val >> 8; ptr[2] = val >> 16; ptr[3] = val >> 24;
    ptr[4] = val >> 32; ptr[5] = val >> 40; ptr[6] = val >> 48; ptr[7] = val >> 56;
    return LONG_SIZE;
}

static unsigned _put_string(char *ptr, const char *str) {
    size_t length = strlen(str);
    unsigned i = _put_ushort(ptr, length);
    const char *c = str;

    for (; i < length + SHRT_SIZE; i++, c++)
        ptr[i] = *c;

    return i;
}

unsigned fcall_msg_to_buf(const fcall *fcall, char *buf, unsigned length) {
    char *ptr = buf;

    unsigned size = fcall_msg_size(fcall);

    if (size == 0 || size > length)
        return 0;

    ptr += _put_uint(ptr, fcall_msg_size(fcall));
    ptr += _put_uchar(ptr, fcall->type);
    ptr += _put_ushort(ptr, fcall->tag);

    switch (fcall->type) {
        case Tversion:
        case Rversion:
            ptr += _put_uint(ptr, fcall->msize);
            ptr += _put_string(ptr, fcall->version);
            break;

        case Tattach:
            ptr += _put_uint(ptr, fcall->fid);
            ptr += _put_uint(ptr, fcall->afid);
            ptr += _put_string(ptr, fcall->uname);
            ptr += _put_string(ptr, fcall->aname);
            break;

        case Rattach:
            ptr += _put_uint(ptr, fcall->qid.type);
            ptr += _put_uint(ptr, fcall->qid.version);
            ptr += _put_ulong(ptr, fcall->qid.id);
            break;

        case Topen:
            ptr += _put_uint(ptr, fcall->fid);
            ptr += _put_uchar(ptr, fcall->mode);
            break;

        case Ropen:
        case Rcreate:
            ptr += _put_uint(ptr, fcall->qid.type);
            ptr += _put_uint(ptr, fcall->qid.version);
            ptr += _put_ulong(ptr, fcall->qid.id);
            ptr += _put_uint(ptr, fcall->iounit);
            break;

        case Tcreate:
            ptr += _put_uint(ptr, fcall->fid);
            ptr += _put_string(ptr, fcall->name);
            ptr += _put_uint(ptr, fcall->perm);
            ptr += _put_uchar(ptr, fcall->mode);
            break;

        case Rerror:
            ptr += _put_string(ptr, fcall->ename);

        default:
            break;
    };

    return ptr - buf;
}
