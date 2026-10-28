#include "kernel/syscalls.h"

#include <kernel/mqueue.h>

long sys_mq_unlink(const char *name) {
    return mqueue_unlink(name);
}
