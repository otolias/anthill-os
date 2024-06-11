#include "stdio.h"

#include "internal/stdio.h"

int fflush(FILE *stream) {
    unsigned res = file_write(stream);

    if (res == 0)
        return EOF;

    return res;
}
