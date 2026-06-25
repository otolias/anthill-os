#include "stdlib.h"

#include "syscalls.h"

void exit(int status) {
    SYSCALL_1(SYS_EXIT, status);
}
