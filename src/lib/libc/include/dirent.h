#ifndef _DIRENT_H
#define _DIRENT_H

#include <stdio.h>
#include <sys/types.h>

typedef struct __file DIR;

struct dirent {
    ino_t d_ino;
    char  d_name[];
};

/*
* Open directory whose path name is equal to the string pointed to by _dirname_.
*
* On success, returns a pointer to the DIR object controlling the stream.
* On failure, returns an null pointer and sets errno to indicate the error.
*
* errno:
* - EACCES Directory not found
* - EMFILE No available file descriptors
*/
DIR *opendir(const char *dirname);

#endif /* _DIRENT_H */
