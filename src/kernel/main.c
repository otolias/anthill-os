#include "debug_printf.h"
#include "uart.h"
#include "irq.h"
#include "timer.h"

void main() {
    uart_init();
    timer_init(2);

    enable_interrupt_controller();
    enable_irq();

    while (1) {;}
}
