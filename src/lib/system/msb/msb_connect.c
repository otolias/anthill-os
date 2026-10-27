#include "msb.h"

#include <errno.h>
#include <syscalls.h>

msb_id msb_connect(msb_id id, const char *name) {
    long res = SYSCALL_2(SYS_MSB_CONN, id, (unsigned long) name);

    if (res < 0) {
        errno = (int) -res;
        return -1;
    }

    return (msb_id) res;
}
