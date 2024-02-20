#include "syscalls.h"

#include <sys/types.h>

long sys_handler(long id, size_t arg1, size_t arg2, size_t arg3, size_t arg4,
                size_t arg5, size_t arg6) {
    long ret;

    __asm__(
        "mov x8, %1 \n"
        "mov x0, %2 \n"
        "mov x1, %3 \n"
        "mov x2, %4 \n"
        "mov x3, %5 \n"
        "mov x4, %6 \n"
        "mov x5, %7 \n"
        "svc #0 \n"
        "mov %w0, w0 \n"
        : "=r" (ret)
        : "r"(id), "r"(arg1), "r"(arg2), "r"(arg3), "r"(arg4), "r"(arg5), "r"(arg6)
        : "memory", "cc"
    );

    return ret;
}
