#include "kernel/syscalls.h"

#include <kernel/task.h>

void sys_exit(int status) {
    task_exit(status);
}
