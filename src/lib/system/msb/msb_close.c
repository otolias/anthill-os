#include "msb.h"

#include <errno.h>
#include <syscalls.h>

int msb_close(msb_id id) {
    int res = SYSCALL_1(SYS_MSB_CLOSE, id);

    if (res < 0) {
        errno = -res;
        return -1;
    }

    return res;
}
