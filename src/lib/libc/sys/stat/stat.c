#include "sys/stat.h"

#include <errno.h>
#include <stdio.h>

int stat(const char *restrict path, struct stat *restrict buf) {
    FILE *f = fopen(path, "r");
    if (!f) {
        errno = ENOENT;
        return -1;
    }

    int res = fstat(f->fid, buf);

    fclose(f);

    return res;
}
