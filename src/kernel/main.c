#include "drivers/uart.h"
#include "drivers/irq.h"
#include "drivers/timer.h"
#include "kernel/kprintf.h"
#include <kernel/ramdisk.h>
#include <kernel/task.h>

void init_process(void) {
    void *file = ramdisk_lookup("./bin/hello");
    short err = task_exec(file);
    if (err)
        kprintf("There was a problem in execution. Exit code: %d\n", err);

    while (1)
        task_schedule();
}

void main(void) {
    timer_init();
    enable_interrupt_controller();
    enable_irq();
    uart_init();

    void *file = ramdisk_lookup("./bin/hello");
    short err = task_exec(file);
    if (err)
        kprintf("Error while starting init process\n");

    while (1)
        task_schedule();
}
