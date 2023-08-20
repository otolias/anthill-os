#include <stdint.h>
#include <stddef.h>

#include "drivers/uart.h"
#include "drivers/irq.h"
#include "drivers/timer.h"
#include "kernel/fork.h"
#include "kernel/kprintf.h"
#include "kernel/scheduler.h"

extern char _binary_build_ramdisk_start;

/*
* A process for testing purposes.
*/
void test_process(char *string) {
    while (1) {
        for (int i = 0; i < 5; i++) {
            uint32_t counter = 100000;

            uart_send_char(string[i]);
            while (counter--) {
                asm volatile("nop");
            }
        }
    }
}

void init_process() {
    preempt_disable();

    // Setup
    uart_init();
    kprintf("%x\n", _binary_build_ramdisk_start);

    preempt_enable();
    while (1) {;}
}

void main() {
    timer_init();
    enable_interrupt_controller();
    enable_irq();

    uint8_t err = move_to_user_mode((uintptr_t) &init_process);
    if (err) { kprintf("Error while starting init process\n"); }

    while (1) {
        schedule();
    }
}
