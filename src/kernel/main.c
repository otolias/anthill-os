#include "debug_printf.h"
#include "uart.h"

void main() {
    int el;

    uart_init();

    asm("mrs x0, CurrentEl;"
        "lsr %[el], x0, #2" :
        [el] "=r" (el) : :
        "x0");

    // Print Exception level
    dbg_printf("Exception Level: %d!\n", el);

    while (1) {
        uart_send_char(uart_get_char());
    }
}
