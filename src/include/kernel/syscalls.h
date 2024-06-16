#ifndef _KERNEL_SYSCALLS_H
#define _KERNEL_SYSCALLS_H

#define SYS_EXIT       0
#define SYS_MMAP       1
#define SYS_MUNMAP     2
#define SYS_MQ_OPEN    3
#define SYS_MQ_CLOSE   4
#define SYS_MQ_UNLINK  5
#define SYS_MQ_SEND    6
#define SYS_MQ_RECV    7
#define SYS_GETPID     8
#define TOTAL_SYSCALLS 9

#ifndef __ASSEMBLER__

extern const void *system_call_table[];

#endif /* __ASSEMBLER__ */
#endif /* _KERNEL_SYSCALLS_H */
