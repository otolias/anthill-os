#include "kernel/syscalls.h"

#include <kernel/errno.h>
#include <kernel/io.h>
#include <kernel/msgbuf.h>
#include <kernel/panic.h>

long sys_msb_send(const int send_id, const char * const recv_name) {
    enum kern_err err = msgbuf_send(send_id, recv_name);
    switch (err) {
        case ERR_OK:
            return 0;

        case ERR_MEM_OOM:
            return -ENOMEM;

        case ERR_MSB_CHF:
            return -EAGAIN;

        case ERR_MSB_FND:
            return -EACCES;

        default:
            io_fmt("Unhandled error code %d\n", err);
            panic("Unhandled error code");
    }
}
