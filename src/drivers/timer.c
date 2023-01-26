#include "timer.h"
#include "debug_printf.h"

void timer_init(uint32_t interval) {
    register uint32_t reg = *TIMER_CONTROL;

    *TIMER_LOAD = interval;

    reg |= 1 << 1; // 32-bit counter
    reg |= 1 << 5; // Enable timer interrupt
    reg |= 1 << 7; // Enable timer
    reg |= 1 << 9; // Enable free running counter
    *TIMER_CONTROL = reg;
    dbg_printf("%x\n", reg);
    dbg_printf("%x\n", *TIMER_CONTROL);
}

void timer_dump() {
    dbg_printf("Timer dump:\n");
    /* dbg_printf("TIMER_LOAD: %x -> %x\n", TIMER_LOAD, *TIMER_LOAD); */
    /* dbg_printf("TIMER_VALUE: %x -> %x\n", TIMER_VALUE, *TIMER_VALUE); */
    /* dbg_printf("TIMER_CONTROL: %x -> %x\n", TIMER_CONTROL, *TIMER_CONTROL); */
    /* dbg_printf("TIMER_ACK : %x -> %x\n", TIMER_ACK, *TIMER_ACK); */
    /* dbg_printf("TIMER_RAW_IRQ : %x -> %x\n", TIMER_RAW_IRQ, *TIMER_RAW_IRQ); */
    /* dbg_printf("TIMER_MASKED_IRQ : %x -> %x\n", TIMER_MASKED_IRQ, *TIMER_MASKED_IRQ); */
    /* dbg_printf("TIMER_RELOAD : %x -> %x\n", TIMER_RELOAD, *TIMER_RELOAD); */
    /* dbg_printf("TIMER_PRE_DIV : %x -> %x\n", TIMER_PRE_DIV, *TIMER_PRE_DIV); */
    dbg_printf("TIMER_FREE_RUNNING : %x -> %x\n", TIMER_FREE_RUNNING, *TIMER_FREE_RUNNING);
}

void handle_timer_irq() {
    *TIMER_ACK = 0;
    dbg_printf("Timer interrupt received\n");
}
