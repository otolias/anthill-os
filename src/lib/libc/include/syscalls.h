#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#include <stddef.h>

#include <kernel/syscalls.h>

#define SYS_WRITE 0
#define SYS_EXIT 1

/* sys handler macro with arguments */
#define SYSCALL_0(id) sys_handler(id, 0);
#define SYSCALL_1(id, arg1) sys_handler(id, arg1);

/* Call syscall _id_ with _arg1_ */
int sys_handler(long id, size_t arg1);

#endif /* _SYSCALLS_H */
