#ifndef _UNISTD_H
#define _UNISTD_H

#include <sys/types.h>

/*
* Return the process ID
*/
pid_t getpid(void);

/*
* Mount connection associated with _fid_ to _name_
*
* On success, returns 0.
* On failure, returns -1 and sets errno to indicate the error.
*
* errno:
* - EACCES Failed to create _name_
* - EBADF  _fid_ is not associated with an open file descriptor
* - EIO    Physical I/O error
* - ENOMEM Not enough available memory
*/
int mount(int fid, const char *restrict name);

#endif /* _UNISTD_H */
