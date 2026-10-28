#include "kernel/syscalls.h"

#include <kernel/mqueue.h>

long sys_mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio) {
    return mqueue_receive(mqdes, msg_ptr, msg_len, msg_prio);
}
