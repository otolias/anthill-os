#include "mqueue.h"

#include <errno.h>
#include <stdarg.h>
#include <stddef.h>
#include <syscalls.h>
#include <sys/types.h>

mqd_t mq_open(const char *name, int oflag, ...) {
    mode_t mode = NULL;
    mq_attr *attr = NULL;
    int access_flags = oflag & 7;

    if (!access_flags || access_flags != (access_flags & -access_flags)) {
        errno = EACCES;
        return -1;
    }

    if (oflag & O_CREAT) {
        va_list args;

        va_start(args, oflag);
        mode = va_arg(args, mode_t);
        attr = va_arg(args, mq_attr*);
        va_end(args);
    }

    int res = SYSCALL_4(SYS_MQ_OPEN, (size_t) name, oflag, mode, (size_t) attr);

    if (res < 0) {
        errno = -res;
        return -1;
    }

    return res;
}

int mq_close(mqd_t mqdes) {
    int res = SYSCALL_1(SYS_MQ_CLOSE, mqdes);

    if (res < 0) {
        errno = -res;
        return -1;
    }

    return res;
}
