#include "dirent.h"

#include <fcntl.h>

#include "internal/stdio.h"

DIR *opendir(const char *dirname) {
    int oflag = O_RDONLY | O_DIRECTORY;
    return file_open(dirname, oflag);
}
