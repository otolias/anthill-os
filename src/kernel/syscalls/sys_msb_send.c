#include "kernel/syscalls.h"

#include <kernel/errno.h>
#include <kernel/io.h>
#include <kernel/msgbuf.h>
#include <kernel/panic.h>

long sys_msb_send(const int send_id, const int recv_id) {
    enum kern_err err = msgbuf_send(send_id, recv_id, 0);
    switch (err) {
        case ERR_OK:
            return 0;

        case ERR_MSB_COD:
            return -ENOENT;

        case ERR_MSB_FND:
            return -EACCES;

        default:
            io_fmt("Unhandled error code %d\n", err);
            panic("Unhandled error code");
    }
}
