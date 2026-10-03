/*
* Message passing implementation
*
* TODO: Timeout for receiving
*/
#ifndef _KERNEL_MSGBUF_H
#define _KERNEL_MSGBUF_H

#include <kernel/error.h>
#include <stddef.h>

#define __MSB_NAME_MAX 32

typedef int msgbuf_id;

struct msgbuf_r_buf {
    char *buf;
    enum kern_err err;
};

struct msgbuf_r_id {
    msgbuf_id id;
    enum kern_err err;
};

/*
* Close message buffer identified by _id_.
*
* Returns enum kern_err.
*/
enum kern_err msgbuf_close(msgbuf_id id);

/*
* Create a message buffer identified by _name_. If _send_buf_ is not NULL,
* allocate an underlying buffer to be used for message sending, and set the
* pointee of _send_buf_ to that buffer.
*
* Returns msgbuf_r_id:
* - On success, _id_ is the new message buffer and _err_ is set to ERR_OK.
* - On failure, _id_ is -1 and _err_ is set to indicate the error.
*/
struct msgbuf_r_id msgbuf_create(const char *name, size_t size, char **send_buf);

/*
* Receive queued message of message buffer identified by _recv_id_. If the
* message buffer's receive queue is full, block until it is not.
*
* TODO: Add flags whether to block or not?
*
* Returns msgbuf_r_buf:
* - On success, _buf_ is the virtual address of the sender's buffer and _err_ is
*   set to ERR_OK.
* - On failure, _buf_ is NULL and _err_ is set to indicate the error.
*/
struct msgbuf_r_buf msgbuf_recv(msgbuf_id recv_id);

/*
* Send message of message buffer identified by _send_id_ to message buffer
* identified by _recv_name_. If the receiver's queue is full, block the task
* until it is available.
*
* Returns kern_err.
*/
enum kern_err msgbuf_send(msgbuf_id send_id, const char *recv_name);

#endif /* _KERNEL_MSGBUF_H */
