#include "kernel/arch/io.h"

#include "uart.h"

int io_write(const unsigned char *data, int n) {
    int i = 0;

    for (; i < n; i++)
        uart_write_char(data[i]);

    return i + 1;
}
