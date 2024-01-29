#ifndef _KERNEL_SYSCALLS_H
#define _KERNEL_SYSCALLS_H

#define TOTAL_SYSCALLS 4

#ifndef __ASSEMBLER__

#include <kernel/sys/types.h>

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

int sys_mq_open(const char *name, int oflag, mode_t mode, void *attr);
int sys_mq_close(int mqdes);

extern const void *system_call_table[];

#endif /* __ASSEMBLER__ */
#endif /* _KERNEL_SYSCALLS_H */
