#include "mqueue.h"

#include <errno.h>
#include <syscalls.h>
#include <sys/types.h>


int mq_unlink(const char *name) {
    int res = SYSCALL_1(SYS_MQ_UNLINK, (size_t) name);

    if (res < 0) {
        errno = -res;
        return -1;
    }

    return res;
}
