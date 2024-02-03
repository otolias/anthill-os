#include "mqueue.h"

#include <errno.h>
#include <syscalls.h>
#include <sys/types.h>

ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio) {
    ssize_t res = SYSCALL_4(SYS_MQ_RECV, mqdes, (size_t) msg_ptr, msg_len, (size_t) msg_prio);

    if (res < 0) {
        errno = -res;
        return -1;
    }

    return res;
}
