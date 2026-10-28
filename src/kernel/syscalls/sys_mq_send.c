#include "kernel/syscalls.h"

#include <kernel/mqueue.h>

long sys_mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio) {
    return mqueue_send(mqdes, msg_ptr, msg_len, msg_prio);
}
