#include "drivers/uart.h"
#include "drivers/irq.h"
#include "drivers/timer.h"
#include <kernel/ramdisk.h>
#include <kernel/task.h>

void main(void) {
    timer_init();
    enable_interrupt_controller();
    enable_irq();
    uart_init();

    void *file;

    file = ramdisk_lookup("./bin/test_suite");
    task_exec(file);

    file = ramdisk_lookup("./modules/mod_rd");
    task_exec(file);

    file = ramdisk_lookup("./modules/mod_vfs");
    task_exec(file);

    file = ramdisk_lookup("./bin/hello");
    task_exec(file);

    while (1)
        task_schedule();
}
