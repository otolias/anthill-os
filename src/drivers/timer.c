#include <stdint.h>

#include <kernel/scheduler.h>
#include <kernel/kprintf.h>

#include "drivers/timer.h"

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

void timer_wait(int msec) {
    uint32_t target = *TIMER_CLO + msec;
    while (*TIMER_CLO < target);
}
