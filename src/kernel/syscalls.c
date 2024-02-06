#include "kernel/syscalls.h"

#include <drivers/uart.h>
#include <kernel/mqueue.h>
#include <kernel/task.h>
#include <kernel/sys/types.h>

const void *system_call_table[] = {
    (void *) sys_write,
    (void *) sys_exit,
    (void *) sys_mq_open,
    (void *) sys_mq_close,
    (void *) sys_mq_unlink,
    (void *) sys_mq_send,
    (void *) sys_mq_receive,
};

int sys_write(char *buffer) {
    int written = 0;
    char c;

    while ((c = (char) *(buffer++))) {
        uart_send_char(c);
        written++;
    }

    return written;
}

void sys_exit(void) {
    task_exit();
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
