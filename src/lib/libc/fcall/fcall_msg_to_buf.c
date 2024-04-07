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
    ptr += put_uint(ptr, fcall->fid);
    ptr += put_ushort(ptr, fcall->tag);

    switch (fcall->type) {
        case Tversion:
        case Rversion:
            ptr += put_uint(ptr, fcall->msize);
            ptr += put_string(ptr, fcall->version, length);
            put_string(ptr, fcall->channel, length);
            break;
        default:
            size = 0;
            break;
    };

    return size;
}
