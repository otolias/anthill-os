#include "mmu.h"

.global mmu_init
mmu_init:
    msr ttbr1_el1, x0
    msr ttbr0_el1, x1

    ldr x0, =MAIR_VALUE
    msr mair_el1, x0

    ldr x0, =TCR_VALUE
    msr tcr_el1, x0
    dsb ish
    isb

    ldr x0, =SCTLR_VALUE
    msr sctlr_el1, x0
    isb

    ret
