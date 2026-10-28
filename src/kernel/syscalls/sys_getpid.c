#include "kernel/syscalls.h"

#include <kernel/task.h>

long sys_getpid(void) {
    return task_current()->pid;
}
