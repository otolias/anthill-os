#include "kernel/syscalls.h"

#include "drivers/uart.h"

const void *system_call_table = {
    sys_write
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
