#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <kernel/syscalls.h>

/* sys handler macro with arguments */
#define SYSCALL_0(id) sys_handler(id, 0, 0, 0, 0, 0, 0);
#define SYSCALL_1(id, arg1) sys_handler(id, arg1, 0, 0, 0, 0, 0);
#define SYSCALL_2(id, arg1, arg2) sys_handler(id, arg1, arg2, 0, 0, 0, 0);
#define SYSCALL_3(id, arg1, arg2, arg3) sys_handler(id, arg1, arg2, arg3, 0, 0, 0);
#define SYSCALL_4(id, arg1, arg2, arg3, arg4) sys_handler(id, arg1, arg2, arg3, arg4, 0, 0);
#define SYSCALL_5(id, arg1, arg2, arg3, arg4, arg5) sys_handler(id, arg1, arg2, arg3, arg4, arg5, 0);
#define SYSCALL_6(id, arg1, arg2, arg3, arg4, arg5, arg6) sys_handler(id, arg1, arg2, arg3, arg4, arg5, arg6);

/* Call syscall _id_ with args */
long sys_handler(long id, long arg1, long arg2, long arg3, long arg4, long arg5,
                 long arg6);

#endif /* _SYSCALLS_H */
