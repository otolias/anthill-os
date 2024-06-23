#include "stdio.h"

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <string.h>

#include "internal/stdio.h"

FILE *fopen(const char *restrict pathname, const char *restrict mode) {
    int oflag, vflag;

    if (strchr(mode, '+')) oflag = O_RDWR;
    else if (strchr(mode, 'r')) oflag = O_RDONLY;
    else if (strchr(mode, 'w')) oflag = O_WRONLY;
    else if (strchr(mode, 'a')) oflag = O_WRONLY;
    else { errno = EINVAL; return NULL; }

    if (strchr(mode, 'w')) oflag |= O_CREAT | O_TRUNC;
    if (strchr(mode, 'a')) oflag |= O_CREAT | O_APPEND;

    /* Flag validation */
    vflag = oflag & (O_RDWR | O_RDONLY | O_WRONLY);

    if (!vflag || vflag != (vflag & -vflag)) {
        errno = EINVAL;
        return NULL;
    }

    vflag = oflag & (O_TRUNC | O_APPEND);

    if (vflag != (vflag & -vflag)) {
        errno = EINVAL;
        return NULL;
    }

    return file_open(pathname, oflag);
}
