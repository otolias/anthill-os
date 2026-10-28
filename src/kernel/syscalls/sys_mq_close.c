#include "kernel/syscalls.h"

#include <kernel/mqueue.h>

long sys_mq_close(mqd_t mqdes) {
    return mqueue_close(mqdes);
}
