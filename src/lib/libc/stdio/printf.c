#include "stdio.h"

#include <stdarg.h>
#include <syscalls.h>

int printf(const char *restrict format, ...) {
    va_list args;
    char buffer[BUFSIZ];

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    return SYSCALL_1(SYS_WRITE, (size_t) buffer);
}
