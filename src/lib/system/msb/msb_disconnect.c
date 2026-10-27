#include "msb.h"

#include <errno.h>
#include <syscalls.h>

int msb_disconnect(msb_id send_id, msb_id recv_id) {
    long res = SYSCALL_2(SYS_MSB_DISCONN, send_id, recv_id);

    if (res < 0) {
        errno = (int) -res;
        return -1;
    }

    return (msb_id) res;
}
