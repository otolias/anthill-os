#ifndef _ENTRY_H
#define _ENTRY_H

/* General and system register size, plus 0x20 that
 * gets added to the sp on svc */
#define STACK_FRAME_SIZE 368

#define SYNC_INVALID_EL1t     0
#define IRQ_INVALID_EL1t      1
#define FIQ_INVALID_EL1t      2
#define ERROR_INVALID_EL1t    3

#define SYNC_INVALID_EL1h     4
#define IRQ_INVALID_EL1h      5
#define FIQ_INVALID_EL1h      6
#define ERROR_INVALID_EL1h    7

#define SYNC_INVALID_EL0_64   8
#define IRQ_INVALID_EL0_64    9
#define FIQ_INVALID_EL0_64    10
#define ERROR_INVALID_EL0_64  11

#define SYNC_INVALID_EL0_32   12
#define IRQ_INVALID_EL0_32    13
#define FIQ_INVALID_EL0_32    14
#define ERROR_INVALID_EL0_32  15

#define SYNC_ERROR            16
#define SYSCALL_ERROR         17

#ifndef __ASSEMBLER__

/*
* Starts a function in user_mode
* Function is stored in x19
* Stack is stored in x20
*/
void start_user();

#endif /* __ASSEMBLER__ */

#endif
