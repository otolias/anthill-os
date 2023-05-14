#ifndef _IRQ_H
#define _IRQ_H

#include <stdint.h>

#include "gpio.h"

#define IRQ_BASIC_PENDING     ((volatile uint32_t*)(MMIO_BASE + 0x0000B200))
#define IRQ_PENDING_1         ((volatile uint32_t*)(MMIO_BASE + 0x0000B204))
#define IRQ_PENDING_2         ((volatile uint32_t*)(MMIO_BASE + 0x0000B208))
#define FIQ_CONTROL           ((volatile uint32_t*)(MMIO_BASE + 0x0000B20C))
#define ENABLE_IRQS_1         ((volatile uint32_t*)(MMIO_BASE + 0x0000B210))
#define ENABLE_IRQS_2         ((volatile uint32_t*)(MMIO_BASE + 0x0000B214))
#define ENABLE_BASIC_IRQS     ((volatile uint32_t*)(MMIO_BASE + 0x0000B218))
#define DISABLE_IRQS_1        ((volatile uint32_t*)(MMIO_BASE + 0x0000B21C))
#define DISABLE_IRQS_2        ((volatile uint32_t*)(MMIO_BASE + 0x0000B220))
#define DISABLE_BASIC_IRQS    ((volatile uint32_t*)(MMIO_BASE + 0x0000B224))

#define SYSTEM_TIMER_IRQ_1    (1 << 1)
#define SYSTEM_TIMER_IRQ_3    (1 << 3)

#define UART_IRQ              (1 << 29)

/*
* Enable interrupts
*/
void enable_interrupt_controller();
/*
* Unmask interrupt requests
*/
void enable_irq();
/*
* IRQ Exception Handler
*/
void handle_irq();
/*
* Mask interrupt requests
*/
void disable_irq();
/*
* Print invalid interrupt message
*/
void show_invalid_entry_message(int32_t type, uint64_t esr, uint64_t address);

#endif
