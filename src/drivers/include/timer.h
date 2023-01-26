#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>
#include "gpio.h"

#define TIMER_LOAD         ((volatile uint32_t*) (MMIO_BASE + 0x0000B400))
#define TIMER_VALUE        ((volatile uint32_t*) (MMIO_BASE + 0x0000B404))
#define TIMER_CONTROL      ((volatile uint32_t*) (MMIO_BASE + 0x0000B408))
#define TIMER_ACK          ((volatile uint32_t*) (MMIO_BASE + 0x0000B40C))
#define TIMER_RAW_IRQ      ((volatile uint32_t*) (MMIO_BASE + 0x0000B410))
#define TIMER_MASKED_IRQ   ((volatile uint32_t*) (MMIO_BASE + 0x0000B414))
#define TIMER_RELOAD       ((volatile uint32_t*) (MMIO_BASE + 0x0000B418))
#define TIMER_PRE_DIV      ((volatile uint32_t*) (MMIO_BASE + 0x0000B41C))
#define TIMER_FREE_RUNNING ((volatile uint32_t*) (MMIO_BASE + 0x0000B420))

void timer_init(uint32_t interval);
void timer_dump();
void handle_timer_irq();

#endif
