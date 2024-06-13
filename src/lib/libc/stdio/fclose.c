#include "stdio.h"

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

#include "internal/stdio.h"

int fclose(FILE *stream) {
    if (stream->buf) {
        fflush(stream);

        free(stream->buf);
        stream->buf = NULL;
    }

    if (!file_close(stream)) {
        errno = EIO;
        return EOF;
    }

    return 0;
}
