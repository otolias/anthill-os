#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <stddef.h>

#define SYS_WRITE     0
#define SYS_EXIT      1
#define SYS_MMAP      2
#define SYS_MQ_OPEN   3
#define SYS_MQ_CLOSE  4
#define SYS_MQ_UNLINK 5
#define SYS_MQ_SEND   6
#define SYS_MQ_RECV   7

/* sys handler macro with arguments */
#define SYSCALL_0(id) sys_handler(id, 0, 0, 0, 0, 0, 0);
#define SYSCALL_1(id, arg1) sys_handler(id, arg1, 0, 0, 0, 0, 0);
#define SYSCALL_2(id, arg1, arg2) sys_handler(id, arg1, arg2, 0, 0, 0, 0);
#define SYSCALL_3(id, arg1, arg2, arg3) sys_handler(id, arg1, arg2, arg3, 0, 0, 0);
#define SYSCALL_4(id, arg1, arg2, arg3, arg4) sys_handler(id, arg1, arg2, arg3, arg4, 0, 0);
#define SYSCALL_5(id, arg1, arg2, arg3, arg4, arg5) sys_handler(id, arg1, arg2, arg3, arg4, arg5, 0);
#define SYSCALL_6(id, arg1, arg2, arg3, arg4, arg5, arg6) sys_handler(id, arg1, arg2, arg3, arg4, arg5, arg6);

/* Call syscall _id_ with args */
long sys_handler(long id, size_t arg1, size_t arg2, size_t arg3, size_t arg4,
                size_t arg5, size_t arg6);

#endif /* _SYSCALLS_H */
