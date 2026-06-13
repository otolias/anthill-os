#include "kernel/syscalls.h"

#include <kernel/mqueue.h>
#include <kernel/sys/types.h>
#include <kernel/task.h>
#include <stddef.h>

mqd_t sys_mq_open(const char *name, int oflag, mode_t mode, void *attr) {
    return mqueue_open(name, oflag, mode, (struct mq_attr *) attr);
}

int sys_mq_close(mqd_t mqdes) {
    return mqueue_close(mqdes);
}

int sys_mq_unlink(const char *name) {
    return mqueue_unlink(name);
}

int sys_mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio) {
    return mqueue_send(mqdes, msg_ptr, msg_len, msg_prio);
}

ssize_t sys_mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio) {
    return mqueue_receive(mqdes, msg_ptr, msg_len, msg_prio);
}

pid_t sys_getpid(void) {
    return task_current()->pid;
}

int sys_spawn(pid_t *pid, void *file, char *const argv[restrict]) {
    // ssize_t res = task_exec(file, argv);
    // *pid = ((struct task *) res)->pid;
    // task_block(task_current()->pid);
    // return res;
}

// Order must be the same as <kernel/syscalls.h>
const void *syscall_table[TOTAL_SYSCALLS] = {
    sys_exit,
    sys_mmap,
    sys_munmap,
    sys_mq_open,
    sys_mq_close,
    sys_mq_unlink,
    sys_mq_send,
    sys_mq_receive,
    sys_getpid,
    sys_spawn,
    sys_fork,
};
