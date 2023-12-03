#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#define TOTAL_SYSCALLS 1

#ifndef __ASSEMBLER__

/*
* Write _buffer_ to UART
*
* Returns the characters written
*/
int sys_write(char *buffer);

extern const void *system_call_table;

#endif /* __ASSEMBLER__ */
#endif /* _SYSCALLS_H */
