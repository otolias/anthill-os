#include "sys/mman.h"

#include <errno.h>
#include <syscalls.h>

void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off) {

    if (!len) {
        errno = EINVAL;
        return MAP_FAILED;
    }

    if (!flags || flags != MAP_ANONYMOUS) {
        errno = EACCES;
        return MAP_FAILED;
    }

    ssize_t res = SYSCALL_6(SYS_MMAP, (size_t) addr, len, prot, flags, fildes, off);

    if (res < 0) {
        errno = -res;
        return MAP_FAILED;
    }

    return (void *) res;
}
