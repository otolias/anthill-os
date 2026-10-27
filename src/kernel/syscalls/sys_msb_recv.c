#include "kernel/syscalls.h"

#include <kernel/errno.h>
#include <kernel/io.h>
#include <kernel/msgbuf.h>
#include <kernel/panic.h>

long sys_msb_recv(msgbuf_id id, char **recv_buf) {
    struct msgbuf_r_id id_r = msgbuf_recv(id, recv_buf);
    switch (id_r.err) {
        case ERR_OK:
            return id_r.id;

        case ERR_MSB_FND:
            return -EACCES;

        default:
            io_fmt("Unhandled error code %d\n", id_r.err);
            panic("Unhandled error code");
    }
}
