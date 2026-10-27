#include "test.h"

#include <kernel/error.h>
#include <kernel/msgbuf.h>
#include <kernel/panic.h>

void _test_run_msgbuf(void) {
    char *send_buf;
    const struct msgbuf_r_id send_r = msgbuf_create("test/sender", 0x1000, &send_buf);
    if (send_r.err != ERR_OK)
        panic("Sender message buffer creation failed");

    const struct msgbuf_r_id recv_r = msgbuf_create("test/receiver", 0, NULL);
    if (recv_r.err != ERR_OK)
        panic("Receiver message buffer creation failed");

    const struct msgbuf_r_id conn_r = msgbuf_connect(send_r.id, "test/receiver");
    if (conn_r.err != ERR_OK)
        panic("Message buffer connection failed");

    *send_buf = 'a';

    if (msgbuf_send(send_r.id, conn_r.id, MSB_NOBLOCK) != ERR_OK)
        panic("Message buffer send failed");

    char *recv_buf;
    const struct msgbuf_r_id id_r = msgbuf_recv(recv_r.id, &recv_buf);
    if (id_r.err != ERR_OK)
        panic("Message buffer receive failed");

    if (*recv_buf != 'a')
        panic("Message buffer receive check failed");

    *recv_buf = 'b';

    if (msgbuf_reply(recv_r.id, id_r.id) != ERR_OK)
        panic("Message buffer reply failed");

    if (*send_buf != 'b')
        panic("Message buffer reply check failed");

    if (msgbuf_disconnect(send_r.id, recv_r.id) != ERR_OK)
        panic("Message buffer disconnection failed");

    struct msgbuf_r_id inv_r;
    inv_r = msgbuf_create("", 0, NULL);
    if (inv_r.err != ERR_MSB_NAM)
        panic("Message buffer name size check failed");

    inv_r = msgbuf_create("test/sender", 0, NULL);
    if (inv_r.err != ERR_MSB_EXS)
        panic("Message buffer name uniqueness check failed");

    inv_r = msgbuf_create("test/invalid", 0, (char **) 0x1000);
    if (inv_r.err != ERR_MSB_SIZ)
        panic("Message buffer size check failed");

    if (msgbuf_send(100, -1, 0) != ERR_MSB_FND)
        panic("Message buffer invalid ID check failed");

    if (msgbuf_close(send_r.id) != ERR_OK)
        panic("Message buffer close failed");

    if (msgbuf_close(recv_r.id) != ERR_OK)
        panic("Message buffer close failed");

    if (msgbuf_close(recv_r.id) != ERR_MSB_FND)
        panic("Message buffer double close check failed");
}
