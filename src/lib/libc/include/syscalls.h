#ifndef _INTERNAL_SYSCALLS_H
#define _INTERNAL_SYSCALLS_H

#include <stddef.h>

#include <kernel/syscalls.h>

#define SYS_WRITE 0

/* Call syscall _id_ with _arg1_ */
int sys_handler(long id, size_t arg1);

#endif /* _INTERNAL_SYSCALLS_H */
