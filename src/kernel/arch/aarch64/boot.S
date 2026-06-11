#include "entry.h"
#include "sysregs.h"

.section ".text.boot"

.global _start
_start:
    // Hang all processors except the first
    mrs x0, mpidr_el1
    and x0, x0, #0xFF
    cbz x0, el1
    b hang

el1:
    ldr x0, =SCTLR_NOMMU_VAL
    msr sctlr_el1, x0

    ldr x0, =HCR_VAL
    msr hcr_el2, x0

    ldr x0, =SPSR_VAL
    msr spsr_el2, x0

    ldr x0, =CPACR_VAL
    msr cpacr_el1, x0

    ldr x0, =vectors
    msr vbar_el1, x0

    adr x0, stack
    msr elr_el2, x0

    eret

stack:
    // Set stack just before _start
    adr x0, _start
    mov sp, x0

    // Clear bss
    ldr x0, =_bss_start
    ldr x1, =_bss_end
    sub x1, x1, x0
bss:
    str  xzr, [x0], #8
    subs x1, x1, #8
    b.gt bss

master:
    // Initialise MMU
    bl mmu_setup
    // Initialise timer
    bl timer_init
    // Initialise uart
    bl uart_init

    // Jump to kernel
    // x0 is the address of the top level user translation table
    ldr x1, =_start
    mov sp, x1

    ldr x1, =main
    blr x1

    // If it returns, hang
hang:
    b hang
