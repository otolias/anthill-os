#include <kernel/arch/io.h>
#include <kernel/arch/irq.h>
#include <kernel/io.h>
#include <kernel/task.h>
#include <stdint.h>

#include "test.h"

void main() {
    mem_setup_finish();

    irq_enable();

    test_run_all();

    io_fmt("Kernel booted successfully...\n");

    while (1)
        task_schedule();
}
