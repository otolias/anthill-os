#include "uart.h"

#include <drivers/uart.h>

unsigned uart_read(unsigned char *data, unsigned n) {
    unsigned i = 0;

    while (1) {
        unsigned timeout = 500;
        while (timeout--) __asm__ volatile("nop");

        if (*AUX_MU_LSR_REG & 0x01) break;
    }

    while (*AUX_MU_LSR_REG & 0x01) {
        if (i == n) break;
        unsigned char c = (unsigned char) *AUX_MU_IO_REG;
        data[i++] = c == '\r' ? '\n' : c;
    }

    return i;
}

unsigned uart_write(const unsigned char* data, unsigned n) {
    unsigned i;

    for (i = 0; i < n; i++) {
        while (!(*AUX_MU_LSR_REG & 0x20))
            __asm__ volatile("nop");

        *AUX_MU_IO_REG = data[i];
    }

    return i;
}
