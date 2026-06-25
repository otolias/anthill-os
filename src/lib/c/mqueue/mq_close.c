#include "mqueue.h"

#include <errno.h>
#include <syscalls.h>

int mq_close(mqd_t mqdes) {
    int res = SYSCALL_1(SYS_MQ_CLOSE, mqdes);

    if (res < 0) {
        errno = -res;
        return -1;
    }

    return res;
}
