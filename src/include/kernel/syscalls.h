#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#define TOTAL_SYSCALLS 4

#ifndef __ASSEMBLER__

#define SYS_WRITE      0

#define SYSCALL_1(id, arg1) sys_handler(id, arg1);

/*
* Calls syscall with _id_
*/
int sys_handler(long id, long arg1);

/*
* Write _buffer_ to UART
*
* Returns the characters written
*/
int sys_write(char *buffer);

extern const void *system_call_table;

#endif /* __ASSEMBLER__ */
#endif /* _SYSCALLS_H */
