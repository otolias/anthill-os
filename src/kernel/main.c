#include <kernel/arch/io.h>
#include <kernel/arch/irq.h>
#include <kernel/io.h>
#include <kernel/task.h>

#include <stdint.h>

void main(void) {
    irq_enable();

    io_fmt("Kernel booted successfully...\n");

    struct task_r_pid forked_r = task_fork();
    if (forked_r.pid != 0) {
        io_fmt("Hello from parent\n");
    } else {
        io_fmt("Hello from child\n");
    }

    while (1)
        task_schedule();
}
