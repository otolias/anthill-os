#include <kernel/arch/io.h>
#include <kernel/arch/irq.h>
#include <kernel/io.h>
#include <kernel/task.h>
#include <stdint.h>

#include "test.h"

void main(uintptr_t *tran_table) {
    // TODO: Unmap kernel segments from user space
    task_current()->tran_table = tran_table;

    irq_enable();

    test_run_all();

    io_fmt("Kernel booted successfully...\n");

    while (1)
        task_schedule();
}
