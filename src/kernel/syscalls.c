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
    return mqueue_open(name, oflag, mode, (mq_attr *) attr);
}

int sys_mq_close(mqd_t mqdes) {
    return mqueue_close(mqdes);
}
