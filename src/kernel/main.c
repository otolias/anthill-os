#include <drivers/irq.h>
#include <drivers/timer.h>
#include <drivers/uart.h>
#include <kernel/kprintf.h>
#include <kernel/mm.h>
#include <kernel/ramdisk.h>
#include <kernel/task.h>

void main(void) {
    mm_init();
    timer_init();
    enable_interrupt_controller();
    enable_irq();
    uart_init();

    void *file;
    ssize_t res;

    file = ramdisk_lookup("./modules/mod_rd");
    res = task_exec(file, NULL);
    if (res > 0)
        kprintf("KERNEL::modules/mod_rd loaded at address %x\n",
                ((struct task *) res)->process_address);
    else {
        kprintf("KERNEL::modules/mod_rd failed to load. Exiting...\n");
        return;
    }

    file = ramdisk_lookup("./modules/mod_uart");
    res = task_exec(file, NULL);
    if (res > 0)
        kprintf("KERNEL::modules/mod_uart loaded at address %x\n",
                ((struct task *) res)->process_address);
    else {
        kprintf("KERNEL::modules/mod_uart failed to load. Exiting...\n");
        return;
    }

    file = ramdisk_lookup("./modules/mod_pl011");
    res = task_exec(file, NULL);
    if (res > 0)
        kprintf("KERNEL::modules/mod_pl011 loaded at address %x\n",
                ((struct task *) res)->process_address);
    else {
        kprintf("KERNEL::modules/mod_pl011 failed to load. Exiting...\n");
        return;
    }

    file = ramdisk_lookup("./modules/mod_vfs");
    res = task_exec(file, NULL);
    if (res > 0)
        kprintf("KERNEL::modules/mod_vfs loaded at address %x\n",
                ((struct task *) res)->process_address);
    else {
        kprintf("KERNEL::modules/mod_vfs failed to load. Exiting...\n");
        return;
    }

    file = ramdisk_lookup("./bin/test_suite");
    res = task_exec(file, NULL);
    if (res > 0)
        kprintf("KERNEL::bin/test_suite loaded at address %x\n",
                ((struct task *) res)->process_address);
    else {
        kprintf("KERNEL::bin/test_suite failed to load. Exiting...\n");
        return;
    }

    file = ramdisk_lookup("./bin/sh");
    res = task_exec(file, NULL);
    if (res > 0)
        kprintf("KERNEL::bin/sh loaded at address %x\n",
                ((struct task *) res)->process_address);
    else {
        kprintf("KERNEL::bin/sh failed to load. Exiting...\n");
        return;
    }

    while (1)
        task_schedule();
}
