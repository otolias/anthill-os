#include "stdio.h"

#include <sys/types.h>

#include "internal/stdio.h"

int fgetc(FILE *stream) {
    if (stream->flags & (1 << F_EOF)) return EOF;

    if (!stream->buf)
        if (file_read(stream, BUFSIZ) == 0) return EOF;

    ssize_t target_chunk = stream->seek_offset / BUFSIZ;
    const char *c = stream->buf + stream->seek_offset % BUFSIZ;

    if (target_chunk != stream->chunk_index || c == stream->buf_end) {
        if (file_read(stream, BUFSIZ) == 0)
            return EOF;
    }

    stream->seek_offset++;

    return *c;
}
