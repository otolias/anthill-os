#include "timer.h"
#include "debug_printf.h"

void timer_init(uint32_t seconds) {
    uint32_t interval = seconds * 1000000;
    uint32_t current_value = *TIMER_CLO + interval;
    *TIMER_C1 = current_value;
}

void handle_timer_irq() {
    *TIMER_CS &= 0x2;
    dbg_printf("Timer interrupt received\n");
}
