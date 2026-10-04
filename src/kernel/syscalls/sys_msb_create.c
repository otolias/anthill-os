#include "kernel/syscalls.h"

#include <kernel/errno.h>
#include <kernel/io.h>
#include <kernel/msgbuf.h>
#include <kernel/panic.h>

long sys_msb_create(const char *name, size_t size, char **send_buf) {
    struct msgbuf_r_id id_r = msgbuf_create(name, size, send_buf);
    switch (id_r.err) {
        case ERR_OK:
            return id_r.id;

        case ERR_MEM_OOM:
            return -ENOMEM;

        case ERR_MSB_EXS:
            return -EEXIST;

        case ERR_MSB_LNG:
            return -ENAMETOOLONG;

        case ERR_MSB_SIZ:
            return -EINVAL;

        default:
            io_fmt("Unhandled error code %d\n", id_r.err);
            panic("Unhandled error code");
    }
}
