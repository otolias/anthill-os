#include "irq.h"
#include "timer.h"
#include "debug_printf.h"

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
    *ENABLE_IRQS_1 |= SYSTEM_TIMER_IRQ_1;
}

void enable_irq() {
    asm volatile("msr daifclr, #2");
}

void handle_irq() {
    uint32_t irq = *IRQ_PENDING_1;
    switch (irq) {
        case (SYSTEM_TIMER_IRQ_1):
            handle_timer_irq();
            break;
        default:
            dbg_printf("Unknown pending irq: %x\n", irq);
    }
}

void disable_irq() {
    asm volatile("msr daifset, #2");
}

void show_invalid_entry_message(int8_t type, uint32_t esr, uint32_t address) {
    dbg_printf("%s, ESR: %x, address: %x\n", entry_error_messages[type], esr, address);
}
