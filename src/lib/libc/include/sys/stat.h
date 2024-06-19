#ifndef _SYS_STAT_H
#define _SYS_STAT_H

#include <sys/types.h>

struct stat {
    off_t st_size;
};

/*
* Put information for file associated with _fildes_ in _buf_.
*
* On success, returns 0.
* On failure, returns -1 and sets errno to indicate the error.
*
* errno:
* - EBADF _fildes_ is not a valid file descriptor
* - EIO   Physical I/O error
*/
int fstat(int fildes, struct stat *buf);

/*
* Put information for _path_ in _buf_.
*
* On success, returns 0.
* On failure, returns -1 and sets errno to indicate the error.
*
* errno:
* - EIO    Physical I/O error
* - ENOENT _path_ does not exist
*/
int stat(const char *restrict path, struct stat *restrict buf);

#endif /* _SYS_STAT_H */
