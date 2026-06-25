#include "stdio.h"

#include <errno.h>

int fileno(FILE *stream) {
    if ((stream->flags & (F_OPEN)) == 0) {
        errno = EBADF;
        return -1;
    }

    return stream->fid;
}
