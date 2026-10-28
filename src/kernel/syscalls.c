#include "kernel/syscalls.h"

const void *syscall_table[TOTAL_SYSCALLS] = {
    [SYS_EXIT] = sys_exit,
    [SYS_FORK] = sys_fork,
    [SYS_GETPID] = sys_getpid,
    [SYS_MMAP] = sys_mmap,
    [SYS_MSB_CLOSE] = sys_msb_close,
    [SYS_MSB_CONN] = sys_msb_connect,
    [SYS_MSB_CREATE] = sys_msb_create,
    [SYS_MSB_DISCONN] = sys_msb_disconnect,
    [SYS_MSB_RECV] = sys_msb_recv,
    [SYS_MSB_REPLY] = sys_msb_reply,
    [SYS_MSB_SEND] = sys_msb_send,
    [SYS_MQ_CLOSE] = sys_mq_close,
    [SYS_MQ_OPEN] = sys_mq_open,
    [SYS_MQ_RECV] = sys_mq_receive,
    [SYS_MQ_SEND] = sys_mq_send,
    [SYS_MQ_UNLINK] = sys_mq_unlink,
    [SYS_MUNMAP] = sys_munmap,
    [SYS_SPAWN] = sys_spawn,
};
