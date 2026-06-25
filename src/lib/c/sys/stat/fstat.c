#include "sys/stat.h"

#include "internal/stdio.h"

int fstat(int fildes, struct stat *buf) {
    return file_get_stat(fildes, buf);
}
