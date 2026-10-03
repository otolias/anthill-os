#include "test.h"

#include <kernel/error.h>
#include <kernel/msgbuf.h>
#include <kernel/panic.h>
#include <kernel/task.h>

void _test_run_msgbuf(void) {
    char *buf;

    const struct msgbuf_r_id send_r = msgbuf_create("test/sender", 0x1000, &buf);
    if (send_r.err != ERR_OK)
        panic("Message buffer sender creation failed");

    const struct msgbuf_r_id recv_r = msgbuf_create("test/receiver", 0, NULL);
    if (recv_r.err != ERR_OK)
        panic("Message buffer receiver creation failed");

    *(buf + 0x100) = 'a';

    if (msgbuf_send(send_r.id, "test/receiver") != ERR_OK)
        panic("Message buffer send failed");

    struct msgbuf_r_buf recv_buf_r = msgbuf_recv(recv_r.id);
    if (recv_buf_r.err != ERR_OK)
        panic("Message buffer receive failed");

    if (*(recv_buf_r.buf + 0x100) != 'a')
        panic("Message buffer received read failed");

    struct msgbuf_r_id inv_r;
    inv_r = msgbuf_create("test/sender", 0, NULL);
    if (inv_r.err != ERR_MSB_EXS)
        panic("Message buffer name uniqueness check failed");

    inv_r = msgbuf_create("test/invalid", 0, (char **) 0x1000);
    if (inv_r.err != ERR_MSB_SIZ)
        panic("Message buffer size check failed");

    if (msgbuf_send(100, "") != ERR_MSB_FND)
        panic("Message buffer invalid ID check failed");

    enum kern_err err = msgbuf_close(send_r.id);
    if (err != ERR_OK)
        panic("Message buffer close failed");

    err = msgbuf_close(recv_r.id);
    if (err != ERR_OK)
        panic("Message buffer close failed");

    err = msgbuf_close(recv_r.id);
    if (err != ERR_MSB_FND)
        panic("Message buffer double close check failed");
}
