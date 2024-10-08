#include "syscalls.h"

long sys_handler(long id, long arg1, long arg2, long arg3, long arg4, long arg5,
                 long arg6) {
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
        "mov %x0, x0 \n"
        : "=r" (ret)
        : "r"(id), "r"(arg1), "r"(arg2), "r"(arg3), "r"(arg4), "r"(arg5), "r"(arg6)
        : "memory", "cc"
    );

    return ret;
}
