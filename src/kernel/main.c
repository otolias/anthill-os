#include <kernel/arch/cpu.h>
#include <kernel/arch/io.h>
#include <kernel/arch/irq.h>
#include <kernel/arch/mem.h>
#include <kernel/io.h>
#include <kernel/mm.h>
#include <kernel/rd.h>
#include <kernel/task.h>

void main(void) {
    mem_init();
    cpu_init();
    io_init();

    irq_enable();

    io_fmt("Kernel booted successfully...\n");

    while (1) {}
}
