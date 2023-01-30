#include "timer.h"
#include "debug_printf.h"

const uint32_t interval = 2000000;
uint32_t current_value = 0;

void timer_init() {
    current_value = *TIMER_CLO;
    current_value += interval;
    *TIMER_C1 = current_value;
}

void handle_timer_irq() {
    current_value += interval;
    *TIMER_C1 = current_value;
    *TIMER_CS = 0x2;
    dbg_printf("Timer interrupt received\n");
}
