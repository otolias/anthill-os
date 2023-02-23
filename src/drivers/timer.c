#include "timer.h"
#include "scheduler.h"

uint32_t interval = 1000000;
uint32_t current_value = 0;

void timer_init() {
    uint32_t current_value = *TIMER_CLO + interval;
    *TIMER_C1 = current_value;
}

void handle_timer_irq() {
    *TIMER_CS &= 0x2;
    current_value = *TIMER_CLO + interval;
    *TIMER_C1 = current_value;
    timer_tick();
}
