#include "msb.h"

#include <errno.h>
#include <syscalls.h>

msb_id msb_create(const char *name, size_t size, char **send_buf) {
    long res = SYSCALL_3(SYS_MSB_CREATE, (long) name, size, (long) send_buf);

    if (res < 0) {
        errno = (int) -res;
        return -1;
    }

    return (msb_id) res;
}
