#include <stdint.h>
#include "uart.h"
#include "irq.h"
#include "timer.h"
#include "fork.h"
#include "scheduler.h"
#include "stdio.h"

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
    printf("Init process\n");
    while (1) {;}
}

void main() {
    uart_init();
    timer_init();

    enable_interrupt_controller();
    enable_irq();

    uint8_t err = move_to_user_mode((uintptr_t) &init_process);
    if (err) { printf("Error while starting init process\n"); }

    while (1) {
        schedule();
    }
}
