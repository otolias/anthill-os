#ifndef _KERNEL_ARCH_AARCH64_GPIO_H
#define _KERNEL_ARCH_AARCH64_GPIO_H

#include <kernel/arch/mem.h>

#define GPFSEL0   ((volatile uint32_t *) (MEM_MMIO + 0x200000))
#define GPFSEL1   ((volatile uint32_t *) (MEM_MMIO + 0x200004))
#define GPFSEL2   ((volatile uint32_t *) (MEM_MMIO + 0x200008))
#define GPFSEL3   ((volatile uint32_t *) (MEM_MMIO + 0x20000C))
#define GPFSEL4   ((volatile uint32_t *) (MEM_MMIO + 0x200010))
#define GPFSEL5   ((volatile uint32_t *) (MEM_MMIO + 0x200014))
#define GPSET0    ((volatile uint32_t *) (MEM_MMIO + 0x20001C))
#define GPSET1    ((volatile uint32_t *) (MEM_MMIO + 0x200020))
#define GPCLR0    ((volatile uint32_t *) (MEM_MMIO + 0x200028))
#define GPLEV0    ((volatile uint32_t *) (MEM_MMIO + 0x200034))
#define GPLEV1    ((volatile uint32_t *) (MEM_MMIO + 0x200038))
#define GPEDS0    ((volatile uint32_t *) (MEM_MMIO + 0x200040))
#define GPEDS1    ((volatile uint32_t *) (MEM_MMIO + 0x200044))
#define GPHEN0    ((volatile uint32_t *) (MEM_MMIO + 0x200064))
#define GPHEN1    ((volatile uint32_t *) (MEM_MMIO + 0x200068))
#define GPPUD     ((volatile uint32_t *) (MEM_MMIO + 0x200094))
#define GPPUDCLK0 ((volatile uint32_t *) (MEM_MMIO + 0x200098))
#define GPPUDCLK1 ((volatile uint32_t *) (MEM_MMIO + 0x20009C))

#endif /* _KERNEL_ARCH_AARCH64_GPIO_H */
