#include "kernel/syscalls.h"

#include <kernel/mqueue.h>

long sys_mq_open(const char *name, int oflag, mode_t mode, void *attr) {
    return mqueue_open(name, oflag, mode, (struct mq_attr *) attr);
}
