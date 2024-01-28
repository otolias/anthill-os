#include "kernel/syscalls.h"

#include <drivers/uart.h>
#include <kernel/mqueue.h>
#include <kernel/task.h>
#include <kernel/sys/types.h>

const void *system_call_table[] = {
    (void *) sys_write,
    (void *) sys_exit,
    (void *) sys_mq_open,
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

int sys_mq_open(const char *name, int oflag, mode_t mode, void *attr) {
    return mqueue_open(name, oflag, mode, (mq_attr *) attr);
}
