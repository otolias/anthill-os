#include "entry.h"

.section ".text.boot"

.global _start
_start:
    // Hang all processors except the first
    mrs     x0, mpidr_el1
    and     x0, x0, #0xFF
    cbz     x0, stack

hang:
    b hang

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
    // Set up exception handlers
    ldr     x0, =vectors
    msr     vbar_el1, x0

    // Jump to kernel
    bl      main
    // If it returns, hang
    b       hang
