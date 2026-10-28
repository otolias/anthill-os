#include "kernel/syscalls.h"

#include <kernel/panic.h>
#include <kernel/task.h>

long sys_spawn(pid_t *pid, void *file, char *const argv[restrict]) {
    panic("Spawn not currently supported");

    // ssize_t res = task_exec(file, argv);
    // *pid = ((struct task *) res)->pid;
    // task_block(task_current()->pid);
    // return res;
}
