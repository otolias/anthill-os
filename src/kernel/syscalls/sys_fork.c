#include "kernel/syscalls.h"

#include <kernel/errno.h>
#include <kernel/error.h>
#include <kernel/io.h>
#include <kernel/panic.h>
#include <kernel/task.h>

pid_t sys_fork(void) {
    struct task_r_pid res = task_fork();
    switch (res.err) {
        case ERR_OK:
            return res.pid;

        case ERR_MEM_OOM:
            return -ENOMEM;

        default:
            io_fmt("Unhandled error code %d\n", res.err);
            panic("Unhandled error code");
    }
}
