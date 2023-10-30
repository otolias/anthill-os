#include "drivers/irq.h"

#include <stdint.h>

#include "drivers/timer.h"
#include "drivers/uart.h"

#include "kernel/kprintf.h"

const char *entry_error_messages[] = {
	"SYNC_INVALID_EL1t",
	"IRQ_INVALID_EL1t",
	"FIQ_INVALID_EL1t",
	"ERROR_INVALID_EL1t",

	"SYNC_INVALID_EL1h",
	"IRQ_INVALID_EL1h",
	"FIQ_INVALID_EL1h",
	"ERROR_INVALID_EL1h",

	"SYNC_INVALID_EL0_64",
	"IRQ_INVALID_EL0_64",
	"FIQ_INVALID_EL0_64",
	"ERROR_INVALID_EL0_64",

	"SYNC_INVALID_EL0_32",
	"IRQ_INVALID_EL0_32",
	"FIQ_INVALID_EL0_32",
	"ERROR_INVALID_EL0_32"
};

void enable_interrupt_controller() {
    *ENABLE_IRQS_1 |= (SYSTEM_TIMER_IRQ_1 | UART_IRQ);
}

void enable_irq() {
    __asm__ volatile("msr daifclr, #2");
}

void handle_irq() {
    uint32_t irq = *IRQ_PENDING_1;
    while (irq) {
        switch (irq) {
            case SYSTEM_TIMER_IRQ_1:
                handle_timer_irq();
                irq &= ~SYSTEM_TIMER_IRQ_1;
                break;
            case UART_IRQ:
                handle_uart_irq();
                irq &= ~UART_IRQ;
                break;
            default:
                debug_log("Unknown pending irq: %x\n", irq);
                break;
        }
    }
}

void disable_irq() {
    __asm__ volatile("msr daifset, #2");
}

void show_invalid_entry_message(int32_t type, uint64_t esr, uint64_t address) {
    kprintf("%s, ESR: %x, address: %x\n", entry_error_messages[type], esr, address);
}
