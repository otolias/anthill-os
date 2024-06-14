#include "pl011.h"

#include <drivers/gpio.h>
#include <stdint.h>

enum pl011_error pl011_init(void) {
    uint32_t reg;

    reg = *GPFSEL3;
    reg &= ~(0x7 << 18 | 0x7 << 21); /* Clear gpio36, gpio37 */
    reg |= (0x6 << 18 | 0x6 << 21); /* Set alt 2 */
    *GPFSEL3 = reg;

    *GPPUD = 0;
    reg = 150;
    while (reg--) { __asm__ volatile("nop"); }
    *GPPUDCLK1 = (1 << 5 | 1 << 6);
    reg = 150;
    while (reg--) { __asm__ volatile("nop"); }
    *GPPUDCLK1 = 0;

    *CR |= 0; /* Disable UART */
    while ((*DR & DR_OE) != 0) __asm__ volatile("nop"); /* Wait until FIFO is empty */
    *LCRH |= ~(1 << 4); /* Flush FIFO */
    *IBRD = 2;
    *FBRD = 0xb; /* Set baud rate to 115200 */
    *LCRH |= (0x3 << 5); /* Set word length to 8 bits */
    *CR |= 1 | (0x3 << 8); /* Set and enable UART */

    return PL011_OK;
}

unsigned pl011_write(const unsigned char *data, unsigned n) {
    unsigned i;

    for (i = 0; i < n; i++) {
        /* Wait until FIFO is empty */
        while ((*DR & DR_OE) != 0)
            __asm__ volatile("nop");

        *DR = data[i];
    }

    return i;
}
