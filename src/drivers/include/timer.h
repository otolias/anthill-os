#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>
#include "gpio.h"

#define TIMER_CS    (volatile uint32_t*) (MMIO_BASE + 0x00003000)
#define TIMER_CLO   (volatile uint32_t*) (MMIO_BASE + 0x00003004)
#define TIMER_CHI   (volatile uint32_t*) (MMIO_BASE + 0x00003008)
#define TIMER_C0    (volatile uint32_t*) (MMIO_BASE + 0x0000300C)
#define TIMER_C1    (volatile uint32_t*) (MMIO_BASE + 0x00003010)
#define TIMER_C2    (volatile uint32_t*) (MMIO_BASE + 0x00003014)
#define TIMER_C3    (volatile uint32_t*) (MMIO_BASE + 0x0000301C)

/*
* Generate System Timer 1 interrupt in given seconds
*/
void timer_init(uint32_t seconds);
/*
* System Timer interrupt handler
*
* Currently handles only System Timer 1 interrupts
*/
void handle_timer_irq();

#endif
