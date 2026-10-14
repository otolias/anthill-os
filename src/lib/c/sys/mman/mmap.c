#include "sys/mman.h"

#include <errno.h>
#include <syscalls.h>

void *mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off) {
    long res = SYSCALL_6(SYS_MMAP, (size_t) addr, len, prot, flags, fildes, off);

    if (res < ERRNO_TOTAL) {
        errno = (int) res;
        return MAP_FAILED;
    }

    return (void *) res;
}
