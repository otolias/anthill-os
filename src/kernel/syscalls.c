#include "kernel/syscalls.h"

#include <drivers/uart.h>
#include <kernel/task.h>

const void *system_call_table[] = {
    (void *) sys_write,
    (void *) sys_exit,
};

int sys_write(char *buffer) {
    int written = 0;
    char c;

    while ((c = (char) *(buffer++))) {
        uart_send_char(c);
        written++;
    }

    return written;
}

void sys_exit(void) {
    task_exit();
}
