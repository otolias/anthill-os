#ifndef _SYS_MMAN_H
#define _SYS_MMAN_H

#include <kernel/syscalls.h>
#include <sys/types.h>

#define PROT_NONE  __PROT_NONE
#define PROT_READ  __PROT_READ
#define PROT_WRITE __PROT_WRITE
#define PROT_EXEC  __PROT_EXEC

#define MAP_FAILED    __MAP_FAILED
#define MAP_ANONYMOUS __MAP_ANONYMOUS
#define MAP_DEVICE    __MAP_DEVICE

/*
* Establish mapping between address space and a memory object.
*
* - _fildes_ is currently unsupported
* - _off_ is currently unsupported
*
* On success, returns a pointer to the start of the mapping.
* On failure, returns MAP_FAILED and sets errno to indicate the error.
*
* Available flags:
* - MAP_ANONYMOUS The mapping is not backed by any file
* - MAP_DEVICE    The mapping corresponds to MMIO memory
*
* errno:
* - EINVAL  The value of _len_ is zero.
* - ENOMEM  No memory available.
* - ENOTSUP Operation is not supported.
*/
void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);

/*
* Remove while page mappings for range starting from address pointed to by
* _addr_ and continuing for _len_ bytes.
*
* On success, returns 0.
* On failure, returns -1 and sets errno to indicate the error.
*
* errno:
* - EINVAL The value of _len_ is zero.
*/
int munmap(void *addr, size_t len);

#endif /* _SYS_MMAN_H */
