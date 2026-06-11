#include <kernel/arch/io.h>
#include <kernel/arch/irq.h>
#include <kernel/io.h>
#include <kernel/rd.h>
#include <kernel/task.h>

#include <stdint.h>

void main(uintptr_t *tran_table) {
    // TODO: Unmap kernel segments from user space
    task_current()->tran_table = tran_table;

    irq_enable();

    io_fmt("Kernel booted successfully...\n");

    struct task_r_pid forked_r = task_fork();
    if (forked_r.err != TASK_OK) {
        io_fmt("KERNEL::Fork failed. Error %d\n", forked_r.err);
        return;
    }

    if (forked_r.pid == 0) {
        void *file = rd_lookup("./bin/hello");
        if (!file) {
            io_fmt("KERNEL::bin/hello not found\n");
            return;
        }

        enum task_err err = task_exec(file, NULL);
        if (err != TASK_OK) {
            io_fmt("KERNEL::bin/hello failed to load. Error %d\n", err);
            return;
        }
    }

    while (1)
        task_schedule();
}
