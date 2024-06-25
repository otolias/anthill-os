#include "stdio.h"

#include <errno.h>
#include <stddef.h>

#include "internal/stdio.h"

int fflush(FILE *stream) {
    if (stream == NULL || !(stream->flags & (F_OPEN | F_WRITE))) {
        errno = EBADF;
        return EOF;
    }

    return file_write(stream);
}
