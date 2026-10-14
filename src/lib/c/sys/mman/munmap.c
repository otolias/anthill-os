#include "sys/mman.h"

#include <errno.h>
#include <sys/types.h>
#include <syscalls.h>

int munmap(void *addr, size_t len) {
    long res = SYSCALL_2(SYS_MUNMAP, (size_t) addr, len);

    if (res < 0) {
        errno = (int) -res;
        return -1;
    }

    return (int) res;
}
