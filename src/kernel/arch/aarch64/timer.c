#include "timer.h"

#include <stdint.h>
#include <kernel/arch/irq.h>
#include <kernel/arch/mem.h>
#include <kernel/task.h>

#define TIMER_CS  ((volatile uint32_t*) (MEM_MMIO + 0x00003000))
#define TIMER_CLO ((volatile uint32_t*) (MEM_MMIO + 0x00003004))
#define TIMER_CHI ((volatile uint32_t*) (MEM_MMIO + 0x00003008))
#define TIMER_C0  ((volatile uint32_t*) (MEM_MMIO + 0x0000300C))
#define TIMER_C1  ((volatile uint32_t*) (MEM_MMIO + 0x00003010))
#define TIMER_C2  ((volatile uint32_t*) (MEM_MMIO + 0x00003014))
#define TIMER_C3  ((volatile uint32_t*) (MEM_MMIO + 0x0000301C))

uint32_t interval = 1000000;
uint32_t current_value = 0;

void timer_init(void) {
    *IRQ_ENABLE_1 |= IRQ_SYSTEM_TIMER_1;
    current_value = *TIMER_CLO + interval;
    *TIMER_C1 = current_value;
}

void timer_irq_handle(void) {
    *TIMER_CS &= 0x2;
    current_value = *TIMER_CLO + interval;
    *TIMER_C1 = current_value;
    task_tick();
}
