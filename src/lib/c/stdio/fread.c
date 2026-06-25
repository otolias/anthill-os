#include "stdio.h"

#include <string.h>

#include "internal/stdio.h"

size_t fread(void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream) {
    if (size == 0 || nitems == 0)
        return 0;

    size_t read = 0;
    size_t remaining_size = size * nitems;

    /* Copy current buffer */
    if (stream->buf && stream->r_end != stream->r_pos) {
        size_t n = stream->r_end - stream->r_pos;
        memcpy(ptr, stream->r_pos, n);
        remaining_size -= n;
        read += n;
    }

    unsigned char *saved_buf = stream->buf;
    unsigned char *saved_buf_pos = stream->r_pos;
    unsigned char *saved_buf_end = stream->r_end;

    stream->buf = ptr;
    stream->r_pos = stream->buf + read;
    stream->r_end = (unsigned char *) ptr + remaining_size;

    read += file_read(stream, size * nitems);

    if (read < remaining_size)
        stream->flags |= F_EOF;

    stream->buf = saved_buf;
    stream->r_pos = saved_buf_pos;
    stream->r_end = saved_buf_end;
    stream->offset += read;

    return read / size;
}
