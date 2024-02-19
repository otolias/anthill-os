#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <sys/types.h>

#define MAP_FAILED 0

#define MAP_ANONYMOUS 1 << 0
/*
* Establish mapping between address space and a memory
* object
*
* - Requesting _addr_ is currently unsupported
* - _prot_ is currently unsupported
* - _fildes_ is currently unsupported
* - _off_ is currently unsupported
*
* On success, returns the address at which the mapping was
* placed.
* On failure, returns MAP_FAILED and sets errno to indicate
* the error.
*
* Available flags:
* - MAP_ANONYMOUS The mapping is not backed by any file
*
* errno:
* - EACCES Unsupported arguments
* - EINVAL The value of _len_ is zero
* - ENOMEM No memory available
*/
void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);

#endif /* _SYS_MMAN_H */
