#include "rd.h"

#include <fcall.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
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

unsigned rd_get_name(const struct header *header, unsigned char *buf, size_t n) {
    char *temp = strdup(header->name);
    char *name = strrchr(temp, '/');

    if (*(name + 1) == 0) {
        *name = 0;
        name = strrchr(temp, '/');
    }

    pstring *res = pstrconv(buf, name + 1, n);
    free(temp);
    return pstrlen(res);
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

unsigned rd_get_stat(const struct header *header, struct fcall_stat *stat, size_t n) {
    unsigned size = 0;
    size += sizeof(stat->qid);
    if (size > n) return 0;

    stat->qid.type = rd_get_type(header);
    stat->qid.version = 0;
    stat->qid.id = 0;

    size += sizeof(stat->length);
    if (size > n) return 0;
    stat->length = rd_get_size(header);

    unsigned char *buf_pos = stat->buffer;
    unsigned res = rd_get_name(header, buf_pos, n - size);
    size += res;
    if (res == 0 || size > n) return 0;

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

unsigned rd_read_dir(const struct header *header, unsigned char *buf, size_t offset,
                     unsigned count, size_t n) {
    size_t name_length = strlen(header->name);
    const struct header *child = header + GET_BLOCKS(rd_get_size(header));
    unsigned written = 0;
    unsigned index = 0;
    unsigned char *buf_pos = buf;

    while (_is_header(child)) {
        if (memcmp(header, child, name_length) != 0 || offset > index) {
            index++;
            child += GET_BLOCKS(rd_get_size(child));
            continue;
        }

        unsigned res = rd_get_stat(child, (struct fcall_stat *) buf_pos, n - written);
        if (res == 0) break;

        written += res;
        buf_pos += res;
        index++;

        if (index == offset - count) break;

        child += GET_BLOCKS(rd_get_size(child));
    }

    return written;
}

unsigned rd_read_file(const struct header *header, unsigned char *buf, size_t offset,
                      unsigned count) {
    size_t file_size = rd_get_size(header);
    unsigned len = offset + count < file_size ? count : file_size - offset;

    const char *data = (char *) (header + 1);

    memcpy(buf, &data[offset], len);
    return len;
}
