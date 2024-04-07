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

unsigned fcall_buf_to_msg(const char *buf, fcall *fcall) {
    const char *ptr = buf;

    unsigned size = get_uint(ptr);
    ptr += INT_SIZE;

    fcall->type = get_uchar(ptr);
    ptr += CHAR_SIZE;
    fcall->fid = get_uint(ptr);
    ptr += INT_SIZE;
    fcall->tag = get_ushort(ptr);
    ptr += SHRT_SIZE;

    switch (fcall->type) {
        case Tversion:
        case Rversion:
            fcall->msize = get_uint(ptr);
            ptr += INT_SIZE;
            fcall->version = (char *) ptr;
            ptr += strlen(ptr) + 1;
            fcall->channel = (char *) ptr;
            break;
        default:
            return 0;
    }

    return size;
}
