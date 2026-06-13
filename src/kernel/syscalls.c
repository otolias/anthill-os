#include "kernel/syscalls.h"

#include <stddef.h>

#include <kernel/errno.h>
#include <kernel/mm.h>
#include <kernel/mqueue.h>
#include <kernel/task.h>
#include <kernel/sys/types.h>

ssize_t sys_mmap(__attribute__((unused)) void *addr, size_t len,
                 __attribute__((unused)) int prot, __attribute__((unused)) int flags,
                 __attribute__((unused)) int fildes, __attribute__((unused)) off_t off) {
    const void *address = mm_get_pages(len);
    if (!address)
        return -ENOMEM;

    return (ssize_t) address;
}

int sys_munmap(void *addr, __attribute__((unused))size_t len) {
    mm_free_pages(addr);
    return 0;
}

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
