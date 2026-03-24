#ifndef _KERNEL_ARCH_AARCH64_TIMER_H
#define _KERNEL_ARCH_AARCH64_TIMER_H

/**
* Setup System Timer 1 and relevant IRQ
*/
void timer_init(void);

/*
* System Timer interrupt handler
*/
void timer_irq_handle();

#endif /* _KERNEL_ARCH_AARCH64_TIMER_H */
