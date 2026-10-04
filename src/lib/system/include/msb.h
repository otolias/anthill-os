#ifndef _LIB_SYSTEM_MSB_H
#define _LIB_SYSTEM_MSB_H

typedef int msb_id;
typedef unsigned long size_t;

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
* - ENAMETOOLONG Message buffer _name_ is too long.
* - EINVAL       _send_buf_ is not NULL, but _size_ is 0.
*/
msb_id msb_create(const char *name, size_t size, char **send_buf);

/*
* Receive queued message of message buffer identified by _recv_id_. If the
* message buffer's receive queue is full, block until it is not.
*
* On success, returns a pointer to the received message.
* On failure, returns NULL and sets errno to indicate the error.
*
* errno:
* - EACCES Message buffer with _recv_id_ doesn't exist.
*/
char* msb_recv(msb_id recv_id);

/*
* Send message of message buffer identified by _send_id_ to message buffer
* identified by _recv_name_. If the receiver's queue is full, block the task
* until it is available.
*
* On success, returns 0.
* On failure, returns -1 and sets errno to indicate the error.
*
* errno:
* - EACCES Message buffer with either _send_id_ or _recv_name_ doesn't exist.
* - EAGAIN Message buffer currently full.
* - ENOMEM Not enough available memory.
*/
int msb_send(msb_id send_id, const char *recv_name);

#endif /* _LIB_SYSTEM_MSB_H */
