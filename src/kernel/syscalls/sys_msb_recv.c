#include "kernel/syscalls.h"

#include <kernel/errno.h>
#include <kernel/io.h>
#include <kernel/msgbuf.h>
#include <kernel/panic.h>

long sys_msb_recv(int recv_id) {
    struct msgbuf_r_buf buf_r = msgbuf_recv(recv_id);
    switch (buf_r.err) {
        case ERR_OK:
            return (long) buf_r.buf;

        case ERR_MSB_FND:
            return EACCES;

        default:
            io_fmt("Unhandled error code %d\n", buf_r.err);
            panic("Unhandled error code");
    }
}
