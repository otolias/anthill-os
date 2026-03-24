#include "kernel/arch/mem.h"

#include "mmu.h"

#include <stdint.h>

void mem_init() {
    uintptr_t *kernel_table = mmu_setup_kernel();
    uintptr_t *user_table = mmu_setup_user();
    mmu_init(kernel_table, user_table);
}
