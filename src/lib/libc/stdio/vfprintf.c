#include "stdio.h"

#include <errno.h>

#include "internal/stdio.h"

int vfprintf(FILE *restrict stream, const char *restrict format, va_list ap) {
    if (!stream->buf) {
        if (!file_alloc(stream))
            return EOF;
    }

    int n = stream->w_end - stream->w_pos;
    int res = formatter(stream->buf, format, ap, n);
    if (res > n) {
        errno = EIO;
        return EOF;
    }

    stream->w_pos += res;

    /* Flush if the last character is a new line or more than half of the buffer is
     * filled */
    if (*(stream->w_pos - 1) == '\n' || stream->w_pos - stream->buf > BUFSIZ / 2) {
        fflush(stream);
    }

    return res;
}
