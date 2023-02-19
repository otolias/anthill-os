#include "sysregs.h"
#include "entry.h"

.section ".text.boot"

.global _start
_start:
    // Hang all processors except the first
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF
    cbz     x0, el1

hang:
    b hang

el1:
    ldr     x0, =SCTLR_VALUE
    msr     sctlr_el1, x0

    ldr     x0, =HCR_VALUE
    msr     hcr_el2, x0

    ldr     x0, =SPSR_VALUE
    msr     spsr_el2, x0

    ldr     x0, =CPACR_VALUE
    msr     cpacr_el1, x0

    ldr     x0, =vectors
    msr     vbar_el1, x0

    adr     x0, stack
    msr     elr_el2, x0

    eret 

stack:
    // Set stack just before _start
    ldr     x1, =_start
    mov     sp, x1

    // Clear bss
    ldr     x1, =bss_start
    ldr     w2, =bss_size
bss:
    cbz     w2, master
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, bss

master:
    // Jump to kernel
    bl      main
    // If it returns, hang
    b       hang
