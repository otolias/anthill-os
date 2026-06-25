/*
* Message queueing
*
* The implementation follows the POSIX standard with some limitations:
*
* - mode_t is unused
* - Message priorities are currently unsupported
*/

#ifndef _MQUEUE_H
#define _MQUEUE_H

#include <fcntl.h>
#include <sys/types.h>

/* Message queue descriptor */
typedef int mqd_t;

/* Message queue attributes */
typedef struct {
    long mq_flags; /* Message queue flags */
    long mq_maxmsg; /* Maximum number of messages */
    long mq_msgsize; /* Maximum message size */
    long mq_curmsg; /* Number of messages currently queued */
} mq_attr;

/*
* Establish a connection to the message queue with
* _name_ and access type specified by _oflag_.
*
* On success, returns the message queue descriptor.
* On failure, returns -1 and sets errno to indicate
* the error.
*
* Available oflags:
*
* Mandatory and exclusive:
* - O_RDONLY   Open queue as read-only
* - O_WRONLY   Open queue as write-only
* - O_RDWR     Open queue as read and write
*
* Optional:
* - O_CREAT    Create new message queue
* - O_EXCL     Fail if the queue name exists
* - O_NONBLOCK Don't block process while waiting
*              for resources
*
* errno:
* - EACCES     The message queue exists but access is
*              denied, or the message queue doesn't
*              exist and creation is denied
* - EEXIST     O_CREAT and O_EXCL are set and the
*              message queue exists
* - EINVAL     O_CREAT is set, attr is not null and
*              mq_maxmsg or mq_msgsize are <= 0
* - ENOENT     Message queue doesn't exist and
*              O_CREAT is not set
* - ENOSPC     Insufficient space for the creation
*              of a new message queue
*/
mqd_t mq_open(const char *name, int oflag, ...);

/*
* Close connection with message queue identified by
* _mqdes_
*
* On success, returns 0.
* On failure, returns -1 and sets errno to indicate
* the error
*
* errno:
* - EBADF  Not a valid message queue descriptor
*/
int mq_close(mqd_t mqdes);

/*
* Remove message queue with _name_. If there are
* pending open connections, destruction is postponed,
* but the function returns succesfully immediately
*
* On success, returns 0.
* On failure, returns -1 and sets errnp to indicate
* the error.
*
* errno:
* - ENOENT  The message queue does not exist
*/
int mq_unlink(const char *name);

/*
* Add the message pointed to by _msg_ptr_ with length
* _msg_len_ to message queue specified by _mqdes_
*
* Message priority is currently unsupported
*
* On success returns 0.
* On failure, returns -1 and sets errno to indicate
* the error.
*
* errno:
* - EACESS    Access is denied (Non-posix)
* - EAGAIN    Specified message queue is full and O_NONBLOCK
*             is set
* - EBADF     Invalid message queue
* - EMSGSIZE  Specified message length exceeds message size
*             attribute
*/
int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio);

/*
* Pop message from the message queue specified bt _mqdes_
* to buffer pointed to by _msg_ptr_ with length _msg_len_
*
* Message priority is currently unsupported
*
* On success, return the length of the message in bytes.
* On failure, returns -1 and sets errno to indicate
* the error.
*
* errno:
* - EACESS    Access is denied (Non-posix)
* - EAGAIN    Message queue is empty and O_NONBLOCK is set
* - EBADF     Invalid message queue
* - EMSGSIZE  Specified message buffer size is less than
*             the message size attribute
*/
ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio);

#endif /* _MQUEUE_H */
