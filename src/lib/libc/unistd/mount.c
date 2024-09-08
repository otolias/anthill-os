#include "unistd.h"

#include <errno.h>

#include "internal/stdio.h"

int mount(int fid, const char *restrict name) {
    FILE *stream = file_find(fid);
    if (!stream) {
        errno = EBADF;
        return -1;
    }

    return file_mount(stream, name);
}
