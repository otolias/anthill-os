#include "drivers/irq.h"
#include "drivers/timer.h"
#include "drivers/uart.h"
#include <kernel/kprintf.h>
#include <kernel/ramdisk.h>
#include <kernel/task.h>

void main(void) {
    timer_init();
    enable_interrupt_controller();
    enable_irq();
    uart_init();

    void *file;
    const void *address;

    file = ramdisk_lookup("./bin/test_suite");
    address = task_exec(file);
    kprintf("bin/test_suite loaded at address %x\n", address);

    file = ramdisk_lookup("./modules/mod_rd");
    address = task_exec(file);
    kprintf("modules/mod_rd loaded at address %x\n", address);

    file = ramdisk_lookup("./modules/mod_vfs");
    address = task_exec(file);
    kprintf("modules/mod_vfs loaded at address %x\n", address);

    file = ramdisk_lookup("./bin/hello");
    address = task_exec(file);
    kprintf("bin/hello loaded at address %x\n", address);

    while (1)
        task_schedule();
}
