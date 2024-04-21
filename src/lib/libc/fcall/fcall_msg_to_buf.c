#include "fcall.h"

static unsigned put_uchar(char *ptr, unsigned short val) {
    ptr[0] = val;
    return CHAR_SIZE;
}

static unsigned put_ushort(char *ptr, unsigned short val) {
    ptr[0] = val; ptr[1] = val >> 8;
    return SHRT_SIZE;
}

static unsigned put_uint(char *ptr, unsigned int val) {
    ptr[0] = val; ptr[1] = val >> 8; ptr[2] = val >> 16; ptr[3] = val >> 24;
    return INT_SIZE;
}

static unsigned put_ulong(char *ptr, unsigned long val) {
    ptr[0] = val; ptr[1] = val >> 8; ptr[2] = val >> 16; ptr[3] = val >> 24;
    ptr[4] = val >> 32; ptr[5] = val >> 40; ptr[6] = val >> 48; ptr[7] = val >> 56;
    return LONG_SIZE;
}

static unsigned put_string(char *ptr, const char *str, unsigned limit) {
    const char *c = str;
    unsigned i;

    for (i = 0; i < limit && *c; i++, c++)
        ptr[i] = *c;

    ptr[i++] = 0;

    return i;
}

unsigned fcall_msg_to_buf(const fcall *fcall, char *buf, unsigned length) {
    char *ptr = buf;

    unsigned size = fcall_msg_size(fcall);

    if (size == 0 || size > length)
        return 0;

    ptr += put_uint(ptr, fcall_msg_size(fcall));
    ptr += put_uchar(ptr, fcall->type);
    ptr += put_ushort(ptr, fcall->tag);

    switch (fcall->type) {
        case Tversion:
        case Rversion:
            ptr += put_uint(ptr, fcall->msize);
            ptr += put_string(ptr, fcall->version, length);
            break;
        case Tattach:
            ptr += put_uint(ptr, fcall->fid);
            ptr += put_uint(ptr, fcall->afid);
            ptr += put_string(ptr, fcall->uname, length);
            ptr += put_string(ptr, fcall->aname, length);
            break;
        case Rattach:
            ptr += put_uint(ptr, fcall->qid.type);
            ptr += put_uint(ptr, fcall->qid.version);
            ptr += put_ulong(ptr, fcall->qid.id);
            break;
        case Tcreate:
            ptr += put_uint(ptr, fcall->fid);
            ptr += put_string(ptr, fcall->name, length);
            ptr += put_uint(ptr, fcall->perm);
            ptr += put_uchar(ptr, fcall->mode);
            break;
        case Rcreate:
            ptr += put_uint(ptr, fcall->qid.type);
            ptr += put_uint(ptr, fcall->qid.version);
            ptr += put_ulong(ptr, fcall->qid.id);
            ptr += put_uint(ptr, fcall->iounit);
            break;
        default:
            break;
    };

    return ptr - buf;
}
