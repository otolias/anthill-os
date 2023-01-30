#include "debug_printf.h"
#include "uart.h"
#include "irq.h"
#include "timer.h"

void main() {
    uint64_t el;

    uart_init();
    timer_init();

    enable_interrupt_controller();
    enable_irq();

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
