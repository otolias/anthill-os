#include <stddef.h>

#include "syscalls.h"

int sys_handler(long id, size_t arg1) {
    int ret;

    __asm__(
        "mov x8, %1 \n"
        "mov x0, %2 \n"
        "svc #0 \n"
        "mov %w0, w0 \n"
        : "=r" (ret)
        : "r"(id), "r"(arg1)
        : "x0"
    );

    return ret;
}
