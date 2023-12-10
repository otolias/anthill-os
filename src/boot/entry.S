#include "boot/entry.h"

#include "boot/sysregs.h"
#include "kernel/syscalls.h"

.macro vector_entry label
.align 7
b   \label
.endm

.macro handle_invalid_entry el, type
kernel_entry \el
mov     x0, #\type
mrs     x1, esr_el1
mrs     x2, elr_el1
bl      show_invalid_entry_message
b       hang
.endm

// Store general registers to stack
.macro kernel_entry el
sub     sp, sp, STACK_FRAME_SIZE
stp     x0, x1, [sp]
stp     x2, x3, [sp, #16]
stp     x4, x5, [sp, #16 * 2]
stp     x6, x7, [sp, #16 * 3]
stp     x8, x9, [sp, #16 * 4]
stp     x10, x11, [sp, #16 * 5]
stp     x12, x13, [sp, #16 * 6]
stp     x14, x15, [sp, #16 * 7]
stp     x16, x17, [sp, #16 * 8]
stp     x18, x19, [sp, #16 * 9]
stp     x20, x21, [sp, #16 * 10]
stp     x22, x23, [sp, #16 * 11]
stp     x24, x25, [sp, #16 * 12]
stp     x26, x27, [sp, #16 * 13]
stp     x28, x29, [sp, #16 * 14]

.if     \el == 0
    mrs     x9, sp_el0
.else
    add     x9, sp, STACK_FRAME_SIZE
.endif /* \el == 0 */

mrs     x10, elr_el1
mrs     x11, spsr_el1

stp     x30, x9, [sp, #16 * 15]
stp     x10, x11, [sp, #16 * 16]
.endm

// Retrieve general registers from stack
.macro  kernel_exit el
ldp     x10, x11, [sp, #16 * 16]
ldp     x30, x9, [sp, #16 * 15]

.if     \el == 0
msr     sp_el0, x9
.endif /* \el == 0 */

msr     elr_el1, x10
msr     spsr_el1, x11

ldp     x0, x1, [sp]
ldp     x2, x3, [sp, #16]
ldp     x4, x5, [sp, #16 * 2]
ldp     x6, x7, [sp, #16 * 3]
ldp     x8, x9, [sp, #16 * 4]
ldp     x10, x11, [sp, #16 * 5]
ldp     x12, x13, [sp, #16 * 6]
ldp     x14, x15, [sp, #16 * 7]
ldp     x16, x17, [sp, #16 * 8]
ldp     x18, x19, [sp, #16 * 9]
ldp     x20, x21, [sp, #16 * 10]
ldp     x22, x23, [sp, #16 * 11]
ldp     x24, x25, [sp, #16 * 12]
ldp     x26, x27, [sp, #16 * 13]
ldp     x28, x29, [sp, #16 * 14]
add     sp, sp, STACK_FRAME_SIZE
eret
.endm

// Exception Vectors
.align 11
.global vectors
vectors:
    vector_entry sync_invalid_el1t
    vector_entry irq_invalid_el1t
    vector_entry fiq_invalid_el1t
    vector_entry error_invalid_el1t

    vector_entry sync_invalid_el1h
    vector_entry el1_irq
    vector_entry fiq_invalid_el1h
    vector_entry error_invalid_el1h

    vector_entry el0_sync
    vector_entry el0_irq
    vector_entry fiq_invalid_el0_64
    vector_entry error_invalid_el0_64

    vector_entry sync_invalid_el0_32
    vector_entry irq_invalid_el0_32
    vector_entry fiq_invalid_el0_32
    vector_entry error_invalid_el0_32

sync_invalid_el1t:
    handle_invalid_entry    1, SYNC_INVALID_EL1t

irq_invalid_el1t:
    handle_invalid_entry    1, IRQ_INVALID_EL1t

fiq_invalid_el1t:
    handle_invalid_entry    1, FIQ_INVALID_EL1t

error_invalid_el1t:
    handle_invalid_entry    1, ERROR_INVALID_EL1t

sync_invalid_el1h:
    handle_invalid_entry    1, SYNC_INVALID_EL1h

el1_irq:
    kernel_entry 1
    bl  handle_irq
    kernel_exit 1

fiq_invalid_el1h:
    handle_invalid_entry    1, FIQ_INVALID_EL1h

error_invalid_el1h:
    handle_invalid_entry    1, ERROR_INVALID_EL1h

el0_sync:
    kernel_entry 0
    mrs     x25, esr_el1
    lsr     x24, x25, #ESR_ELx_EC
    cmp     x24, #ESR_ELx_EC_SVC64
    b.eq    el0_svc
    handle_invalid_entry 0, SYNC_ERROR

el0_irq:
    kernel_entry 0
    bl  handle_irq
    kernel_exit 0

fiq_invalid_el0_64:
    handle_invalid_entry    0, FIQ_INVALID_EL0_64

error_invalid_el0_64:
    handle_invalid_entry    0, ERROR_INVALID_EL0_64

sync_invalid_el0_32:
    handle_invalid_entry    0, SYNC_INVALID_EL0_32

irq_invalid_el0_32:
    handle_invalid_entry    0, IRQ_INVALID_EL0_32

fiq_invalid_el0_32:
    handle_invalid_entry    0, FIQ_INVALID_EL0_32

error_invalid_el0_32:
    handle_invalid_entry    0, ERROR_INVALID_EL0_32

el0_svc:
    adr     x25, system_call_table
    uxtw    x26, w8
    bl      enable_irq
    cmp     x26, #TOTAL_SYSCALLS
    b.hs    syscall_error

    // Find system call in system call table
    ldr     x16, [x25, x26, lsl#3]
    // Execute system call
    blr     x16
    bl      disable_irq
    str     x0, [sp]
    kernel_exit 0

syscall_error:
    handle_invalid_entry    0, SYSCALL_ERROR

.global start_user
start_user:
    bl disable_irq
    msr     elr_el1, x19
    msr     sp_el0, x20
    eret

hang:
    b hang
