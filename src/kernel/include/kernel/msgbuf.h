/*
* Zero-copy message passing
*
* TODO: Timeout for receiving
* TODO: Add non-blocking flag for msb_recv
*/
#ifndef _KERNEL_MSGBUF_H
#define _KERNEL_MSGBUF_H

#include <kernel/error.h>
#include <stddef.h>

#define __MSB_NAME_MAX 32

typedef int msgbuf_id;

enum msgbuf_send_flags {
    MSB_NOBLOCK = 1,
};

struct msgbuf_r_buf {
    char *buf;
    enum kern_err err;
};

struct msgbuf_r_id {
    msgbuf_id id;
    enum kern_err err;
};

/*
* Disconnect all connections and close message buffer identified by _id_.
*
* Returns enum kern_err.
*/
enum kern_err msgbuf_close(msgbuf_id id);

/*
* Connect message buffer identified by _id_ to message buffer identified by
* _name_.
*
* Returns msgbuf_r_id:
* - On success, _id_ is the other message buffer's ID and _err_ is set to
*   ERR_OK.
* - On failure, _id_ is -1 and _err_ is set to indicate the error.
*/
struct msgbuf_r_id msgbuf_connect(msgbuf_id id, const char *name);

/*
* Create a message buffer identified by _name_. If _send_buf_ is not NULL,
* allocate an underlying buffer for sending messaged, and set the pointee of
* _send_buf_ to that buffer.
*
* Returns msgbuf_r_id:
* - On success, _id_ is the new message buffer ID and _err_ is set to ERR_OK.
* - On failure, _id_ is -1 and _err_ is set to indicate the error.
*/
struct msgbuf_r_id msgbuf_create(const char *name, size_t size, char **send_buf);

/*
* Disconnect message buffer identified by _send_id_ from message buffer
* identified by _recv_id_.
*
* Returns enum kern_err.
*/
enum kern_err msgbuf_disconnect(msgbuf_id send_id, msgbuf_id recv_id);

/*
* Receive queued message of message buffer identified by _id_ and set the
* pointee of _recv_buf_ to the underlying buffer. If the message buffer's
* receive queue is empty, block until it is not.
*
* Returns msgbuf_r_buf:
* - On success, _id_ is the ID of the sender and _err_ is set to ERR_OK.
* - On failure, _id_ is -1 and _err_ is set to indicate the error.
*/
struct msgbuf_r_id msgbuf_recv(msgbuf_id id, char **recv_buf);

/*
* Unblock owner of _reply_id_.
*
* Returns enum kern_err.
*/
enum kern_err msgbuf_reply(msgbuf_id id, msgbuf_id reply_id);

/*
* Send message of message buffer identified by _send_id_ to message buffer
* identified by _recv_id_ and block until a reply has been given.
*
* ONLY FOR KERNEL TESTING: If _flags_ is set to MSB_NOBLOCK, returns with ERR_OK
* immediately after sending.
*
* Returns kern_err.
*/
enum kern_err msgbuf_send(msgbuf_id send_id, msgbuf_id recv_id, enum msgbuf_send_flags flags);

#endif /* _KERNEL_MSGBUF_H */
