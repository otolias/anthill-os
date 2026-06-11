#include "kernel/arch/irq.h"

#include "timer.h"

#include <kernel/io.h>
#include <kernel/task.h>
#include <stdint.h>

static const char *entry_error_messages[] = {
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
    "ERROR_INVALID_EL0_32",

    "SYNC_ERROR",
    "SYSCALL_ERROR",
    "DATA_ABORT_ERROR"
};

void irq_handle(void) {
    uint32_t irq = *IRQ_PENDING_1;
    while (irq) {
        switch (irq) {
            case IRQ_SYSTEM_TIMER_1:
                timer_irq_handle();
                irq &= ~IRQ_SYSTEM_TIMER_1;
                break;

            default:
                io_fmt("Unknown pending irq: %x\n", irq);
                break;
        }
    }
}

void irq_invalid_message(int32_t type, uint64_t esr, uint64_t address) {
    io_fmt("%s, ESR: 0x%x, address: 0x%x\n", entry_error_messages[type], esr, address);
}
