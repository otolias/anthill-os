#include "stdio.h"

#include <errno.h>

#include "internal/stdio.h"

int fputc(int c, FILE *stream) {
    if (!stream->buf) {
        if (!file_alloc(stream))
            return EOF;
    }

    if (stream->buf == stream->w_end) {
        errno = EIO;
        return EOF;
    }

    *stream->w_pos++ = c;

    return c;
}
