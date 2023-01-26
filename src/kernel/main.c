#include "debug_printf.h"
#include "uart.h"
#include "irq.h"
#include "timer.h"

void main() {
    int el;

    uart_init();
    enable_interrupt_controller();
    enable_irq();

    timer_init(200);

    timer_dump();

    asm("mrs x0, CurrentEl;"
        "lsr %[el], x0, #2" :
        [el] "=r" (el) : :
        "x0");

    // Print Exception level
    dbg_printf("Exception Level: %d!\n", el);

    timer_dump();

    while (1) {
        uart_send_char(uart_get_char());
    }
}
