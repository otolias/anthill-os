#include "msb.h"

#include <errno.h>
#include <syscalls.h>

void* msb_recv(msb_id id, void * (*callback)(const char *recv_buf)) {
    const char *buf;
    msb_id recv_id = SYSCALL_2(SYS_MSB_RECV, id, (long) &buf);

    if (recv_id < 0) {
        errno = (int) -recv_id;
        return MSB_FAIL;
    }

    callback(buf);

    long res = SYSCALL_2(SYS_MSB_REPLY, id, recv_id);

    if (res < 0) {
        errno = (int) -res;
        return MSB_FAIL;
    }

    return 0;
}
