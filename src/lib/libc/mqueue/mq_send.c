#include "mqueue.h"

#include <errno.h>
#include <syscalls.h>
#include <sys/types.h>

int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio) {
    int res = SYSCALL_4(SYS_MQ_SEND, mqdes, (size_t) msg_ptr, msg_len, msg_prio);

    if (res < 0) {
        errno = -res;
        return -1;
    }

    return res;
}
