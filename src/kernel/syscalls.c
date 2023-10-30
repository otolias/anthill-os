#include "kernel/syscalls.h"

#include "drivers/uart.h"

const void *system_call_table = {
    sys_write
};

int sys_handler(long id, long arg1) {
    int ret;

    __asm(
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

int sys_write(char *buffer) {
    int written = 0;
    char c;

    while ((c = (char) *(buffer++))) {
        uart_send_char(c);
        written++;
    }

    return written;
}
