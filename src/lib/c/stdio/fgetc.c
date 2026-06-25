#include "stdio.h"

#include <errno.h>
#include <sys/types.h>

#include "internal/stdio.h"

int fgetc(FILE *stream) {
    if (stream->flags & (F_EOF)) return EOF;
    if (!(stream->flags & (F_READ))) { errno = EBADF; return EOF; }

    if (stream->r_pos == stream->r_end) {
        if (file_read(stream, BUFSIZ) == 0)
            return EOF;
    }

    if (stream->offset != -1) stream->offset++;

    return *stream->r_pos++;
}
