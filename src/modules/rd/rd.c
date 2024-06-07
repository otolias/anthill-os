#include "rd.h"

#include <fcall.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#define GET_BLOCKS(size) (((size) + 511) / 512) + 1

const char *ramdisk = (char *) 0x3e000000;

/*
* Returns if _header_ is a valid tar header
*/
static bool _is_header(const struct header *header) {
    return memcmp(header->magic, "ustar", 6) == 0;
}

struct header* rd_find(const pstring *path) {
    struct header *header = (struct header *) ramdisk;

    while(_is_header(header)) {
        if (memcmp(path->s, header->name + 1, path->len) == 0)
            break;

        header += GET_BLOCKS(rd_get_size(header));
    }

    if (!_is_header(header))
        return NULL;

    return header;
}

size_t rd_get_size(const struct header *header) {
    if (!_is_header(header))
        return 0;

    off_t size = 0;
    int total = sizeof(header->size);
    const char *c = header->size;

    while(total-- && *c) {
        size *= 8;
        size += *c - '0';
        c++;
    }

    return size;
}

unsigned int rd_get_type(const struct header *header) {
    switch(header->type) {
        case RDT_FILE:
        case RDT_HLINK:
        case RDT_SLINK:
        case RDT_CHDEV:
        case RDT_BLCK:
        case RDT_FIFO:
            return QTFILE;

        case RDT_DIR:
            return QTDIR;

        default:
            return ~0;
    }
}

unsigned rd_read(const struct header *header, char *buf, size_t offset, unsigned count) {
    size_t file_size = rd_get_size(header);
    unsigned len = offset + count < file_size ? count : file_size - offset;

    const char *data = (char *) (header + 1);

    memcpy(buf, &data[offset], len);
    return len;
}
