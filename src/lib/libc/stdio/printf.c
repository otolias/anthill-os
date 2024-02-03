#include "stdio.h"

#include <stdarg.h>
#include <syscalls.h>

#include "internal/stdio.h"

int printf(const char *restrict format, ...) {
    va_list args;
    char buffer[PRINTF_BUFFER_SIZE];

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    return SYSCALL_1(SYS_WRITE, (size_t) buffer);
}
