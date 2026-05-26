#ifndef _KERNEL_ARCH_AARCH64_IRQ_H
#define _KERNEL_ARCH_AARCH64_IRQ_H

#include <kernel/arch/mem.h>

#define IRQ_BASIC_PENDING  ((volatile uint32_t*) (MEM_MMIO + 0x0000B200))
#define IRQ_PENDING_2      ((volatile uint32_t*) (MEM_MMIO + 0x0000B208))
#define FIQ_CONTROL        ((volatile uint32_t*) (MEM_MMIO + 0x0000B20C))
#define ENABLE_IRQS_2      ((volatile uint32_t*) (MEM_MMIO + 0x0000B214))
#define ENABLE_BASIC_IRQS  ((volatile uint32_t*) (MEM_MMIO + 0x0000B218))
#define DISABLE_IRQS_1     ((volatile uint32_t*) (MEM_MMIO + 0x0000B21C))
#define DISABLE_IRQS_2     ((volatile uint32_t*) (MEM_MMIO + 0x0000B220))
#define DISABLE_BASIC_IRQS ((volatile uint32_t*) (MEM_MMIO + 0x0000B224))

#define IRQ_PENDING_1 ((volatile uint32_t*) (MEM_MMIO + 0x0000B204))
#define IRQ_ENABLE_1  ((volatile uint32_t*) (MEM_MMIO + 0x0000B210))

#define IRQ_SYSTEM_TIMER_1 (1 << 1)

/*
* Mask interrupt requests
*/
void irq_disable(void);

/*
* Unmask interrupt requests
*/
void irq_enable(void);

/*
* Handle interrupt request
*/
void irq_handle(void);

#endif /* _KERNEL_ARCH_AARCH64_IRQ_H */
