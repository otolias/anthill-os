#include "kernel/syscalls.h"

#include <kernel/errno.h>
#include <kernel/io.h>
#include <kernel/msgbuf.h>
#include <kernel/panic.h>

long sys_msb_connect(int id, const char *name) {
    struct msgbuf_r_id id_r = msgbuf_connect(id, name);
    switch (id_r.err) {
        case ERR_OK:
            return id_r.id;

        case ERR_MEM_OOM:
            return -ENOMEM;

        case ERR_MSB_COE:
            return -EEXIST;

        case ERR_MSB_COF:
            return -EAGAIN;

        case ERR_MSB_FND:
            return -EACCES;

        default:
            io_fmt("Unhandled error code %d\n", id_r.err);
            panic("Unhandled error code");
    }
}
