#include "msb.h"

#include <errno.h>
#include <syscalls.h>

int msb_send(msb_id send_id, const char *recv_name) {
    long res = SYSCALL_2(SYS_MSB_SEND, send_id, (long) recv_name);

    if (res < 0) {
        errno = (int) -res;
        return -1;
    }

    return (msb_id) res;
}
