/*
* Message queueing implementation
*/

#ifndef _KERNEL_MQUEUE_H
#define _KERNEL_MQUEUE_H

#include <kernel/sys/types.h>

typedef int mqd_t;

typedef struct {
    long mq_flags; /* Message queue flags */
    long mq_maxmsg; /* Maximum number of messages */
    long mq_msgsize; /* Maximum message size */
    long mq_curmsg; /* Number of messages currently queued; */
} mq_attr;

#define O_RDONLY 1 << 0
#define O_WRONLY 1 << 1
#define O_RDWR   1 << 2
#define O_CREAT  1 << 3
#define O_EXCL   1 << 4

/*
* Open mqueue connection
*
* On success, returns message queue id
* On failure, returns -errno
*/
mqd_t mqueue_open(const char *name, int oflag, mode_t mode, const mq_attr *attr);

/*
* Close mqueue connection with id _mqdes_
*
* On success, returns 0
* On failure, returns -errno
*/
int mqueue_close(mqd_t mqdes);

/*
* Remove mqueue. If there are pending connections,
* mark queue as closed and return
*
* On success, returns 0
* On failure, returns -errno
*/
int mqueue_unlink(const char *name);

#endif /* _KERNEL_MQUEUE_H */
