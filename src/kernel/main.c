#include <stdint.h>
#include "debug_printf.h"
#include "uart.h"
#include "irq.h"
#include "timer.h"
#include "fork.h"
#include "scheduler.h"

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

void main() {
    uart_init();
    timer_init();

    enable_interrupt_controller();
    enable_irq();

    uint8_t res = copy_process((uintptr_t) &test_process, (uintptr_t) "12345");
    if (res) { dbg_printf("Error while starting process 1\n"); return; }

    res = copy_process((uintptr_t) &test_process, (uintptr_t) "abcde");
    if (res) { dbg_printf("Error while starting process 2\n"); return; }

    while (1) {
        schedule();
    }
}
