#include "unistd.h"

#include <syscalls.h>

pid_t getpid(void) {
    return SYSCALL_0(SYS_GETPID);
}
