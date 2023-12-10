#ifndef _KERNEL_SYSCALLS_H
#define _KERNEL_SYSCALLS_H

#define TOTAL_SYSCALLS 2

#ifndef __ASSEMBLER__

/*
* Write _buffer_ to UART
*
* Returns the characters written
*/
int sys_write(char *buffer);

/*
* Terminate process
*/
void sys_exit();

extern const void *system_call_table[];

#endif /* __ASSEMBLER__ */
#endif /* _KERNEL_SYSCALLS_H */
