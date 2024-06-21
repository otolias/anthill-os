#include "stdio.h"

#include <errno.h>

#include "internal/stdio.h"

int fclose(FILE *stream) {
    if (stream->buf && stream->flags & (1 << F_WRITE) && stream->w_pos != stream->w_end)
        fflush(stream);

    if (!file_close(stream)) {
        errno = EIO;
        return EOF;
    }

    return 0;
}
