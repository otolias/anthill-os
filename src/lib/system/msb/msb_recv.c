#include "msb.h"

#include <errno.h>
#include <syscalls.h>

char* msb_recv(msb_id recv_id) {
    long res = SYSCALL_1(SYS_MSB_RECV, recv_id);

    if (res < ERRNO_TOTAL) {
        errno = (int) res;
        return 0;
    }

    return (char *) res;
}
