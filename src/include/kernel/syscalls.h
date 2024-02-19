#ifndef _KERNEL_SYSCALLS_H
#define _KERNEL_SYSCALLS_H

#define SYS_WRITE      0
#define SYS_EXIT       1
#define SYS_MMAP       2
#define SYS_MUNMAP     3
#define SYS_MQ_OPEN    4
#define SYS_MQ_CLOSE   5
#define SYS_MQ_UNLINK  6
#define SYS_MQ_SEND    7
#define SYS_MQ_RECV    8
#define TOTAL_SYSCALLS 9

#ifndef __ASSEMBLER__

#include <stddef.h>

#include <kernel/mqueue.h>
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

ssize_t sys_mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);
int sys_munmap(void *addr, size_t len);

int sys_mq_open(const char *name, int oflag, mode_t mode, void *attr);
int sys_mq_close(int mqdes);
int sys_mq_unlink(const char *name);
int sys_mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio);
ssize_t sys_mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio);

extern const void *system_call_table[];

#endif /* __ASSEMBLER__ */
#endif /* _KERNEL_SYSCALLS_H */
