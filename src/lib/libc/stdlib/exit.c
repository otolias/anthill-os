#include "stdlib.h"

#include "syscalls.h"

void exit() {
    SYSCALL_0(SYS_EXIT);
}
