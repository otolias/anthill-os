#ifndef _LIB_SYSTEM_MSB_H
#define _LIB_SYSTEM_MSB_H

typedef int msb_id;
typedef unsigned long size_t;

#define MSB_FAIL ((void *) -1)

/*
* Close message buffer identified by _id_
*
* On success, returns 0.
* On failure, returns -1 and sets errno to indicate the error.
*
* errno:
* - EACCES Message buffer with _id_ doesn't exist.
*/
int msb_close(msb_id id);

/*
* Connect the message buffer identified by _id_ with message buffer identified
* by _name_.
*
* On success, returns the receiver buffer's ID.
* On failure, returns -1 and sets errno to indicate the error.
*
* errno:
* - EEXIST Connection already exists
* - EACCES Message buffer with _id_ or with _name_ doesn't exist.
* - EAGAIN Message buffer has too many connections
* - ENOMEM Not enough available memory
*/
msb_id msb_connect(msb_id id, const char *name);

/*
* Create a message buffer identified by _name_. If _send_buf_ is not NULL,
* allocate an underlying buffer to be used for message sending, and set the
* pointee of _send_buf_ to that buffer.
*
* On success, returns the new buffer's ID.
* On failure, returns -1 and sets errno to indicate the error.
*
* errno:
* - EEXIST       Message buffer with _name_ already exists.
* - ENOMEM       Not enough available memory.
* - EINVAL       Either _name_ is invalid or _send_buf_ is not NULL, while
*                _size_ is 0.
*/
msb_id msb_create(const char *name, size_t size, char **send_buf);

/*
* Disconnect the message buffer identified by _send_id_ from the message buffer
* identified by _recv_id_.
*
* On success, returns 0.
* On failure, returns -1 and sets errno to indicate the error.
*
* errno:
* - EACCES Message buffer with either _send_id_ or _recv_id_ doesn't exist.
* - ENOENT Connection between between message buffers doesn't exist.
*/
int msb_disconnect(msb_id send_id, msb_id recv_id);

/*
* Receive queued message of message buffer identified by _recv_id. If the
* messag buffer's receive queue is empty, block until it is not. When a message
* is received, execute the function pointed to by _callback_, which accepts the
* underlying buffer as a sole argument.
*
* On success, returns the _callback_'s return value.
* On failure, returns MSB_FAIL and sets errno to indicate the error.
*
* errno:
* - EACCES Message buffer with _id_ doesn't exist.
*/
void* msb_recv(msb_id id, void * (*callback)(const char *recv_buf));

/*
* Send message of message buffer identified by _send_id_ to message buffer
* identified by _recv_id_ and block until a reply has been written to the
* underlying buffer.
*
* On success, returns 0.
* On failure, returns -1 and sets errno to indicate the error.
*
* errno:
* - EACCES Message buffer with either _send_id_ or _recv_id doesn't exist.
* - ENOENT Connection doesn't exist.
*/
int msb_send(msb_id send_id, msb_id recv_id);

#endif /* _LIB_SYSTEM_MSB_H */
