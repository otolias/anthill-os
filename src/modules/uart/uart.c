#include "uart.h"

#include <drivers/uart.h>

unsigned uart_write(const unsigned char* data, unsigned n) {
    unsigned i;

    for (i = 0; i < n; i++) {
        while (!(*AUX_MU_LSR_REG & 0x20))
            __asm__ volatile("nop");

        *AUX_MU_IO_REG = data[i];
    }

    return i;
}
