#include "kernel/arch/cpu.h"

#include "timer.h"

void cpu_init(void) {
    timer_init();
}
