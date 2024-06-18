#include "stdio.h"

#include <string.h>

#include "internal/stdio.h"

size_t fread(void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream) {
    if (size == 0 || nitems == 0)
        return 0;

    size_t read = 0;
    size_t remaining_size = size * nitems;

    /* Copy current buffer */
    if (stream->buf && stream->buf_end != stream->buf_pos) {
        size_t n = stream->buf_end - stream->buf_pos;
        memcpy(ptr, stream->buf_pos, n);
        remaining_size -= n;
        read += n;
    }

    char *saved_buf = stream->buf;
    char *saved_buf_pos = stream->buf_pos;
    char *saved_buf_end = stream->buf_end;

    stream->buf = ptr;
    stream->buf_pos = stream->buf + read;
    stream->buf_end = (char *) ptr + remaining_size;

    read += file_read(stream, size * nitems);

    if (read < remaining_size)
        stream->flags |= 1 << F_EOF;

    stream->buf = saved_buf;
    stream->buf_pos = saved_buf_pos;
    stream->buf_end = saved_buf_end;
    stream->seek_offset += read;

    return read / size;
}
