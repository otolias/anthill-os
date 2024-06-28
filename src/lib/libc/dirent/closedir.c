#include "dirent.h"

#include <errno.h>

#include "internal/stdio.h"

int closedir(DIR *dirp) {
    if (!(dirp->flags & F_OPEN)) {
        errno = EBADF;
        return -1;
    }

    if (!file_close(dirp)) {
        errno = EIO;
        return EOF;
    }

    return 0;
}
