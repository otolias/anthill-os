#include "kernel/syscalls.h"

#include <kernel/errno.h>
#include <kernel/task.h>

pid_t sys_fork(void) {
    struct task_r_pid res = task_fork();
    switch (res.err) {
        case TASK_OK:
            return res.pid;

        case TASK_ERR_MEM:
            return -ENOMEM;

        default:
            return -EUNKNOWN;
    }
}
