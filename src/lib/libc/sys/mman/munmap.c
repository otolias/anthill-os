#include "sys/mman.h"

#include <errno.h>
#include <syscalls.h>
#include <sys/types.h>

int munmap(void *addr, size_t len) {
    int res = SYSCALL_2(SYS_MUNMAP, (size_t) addr, len);

    if (res < 0) {
        errno = -res;
        return -1;
    }

    return res;
}
