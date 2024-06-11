/*
* VFS message handling functions and macros
*/
#ifndef _SYS_VFS_H
#define _SYS_VFS_H

#include <fcall.h>
#include <mqueue.h>

#define VFS_MAX_MSG_LEN 128
#define VFS_MAX_IOUNIT 112

struct vfs_msg {
    fcall fcall; /* Fcall struct */
    mqd_t mq_id; /* Message queue to respond */
};

/*
* Parse _msg_ from _buf_. Returns number of bytes parsed.
*/
unsigned vfs_msg_parse(struct vfs_msg *msg, char* buf);

/*
* Put _msg_ to _buf_. Returns number of bytes put.
*/
unsigned vfs_msg_put(struct vfs_msg *msg, char *buf);

/*
* Send _msg_ to _out_ and receive response from _in_. Returns the number
* of bytes of the response on success, or zero on failure
*/
unsigned vfs_msg_send(struct vfs_msg *msg, mqd_t out, mqd_t in);

#endif /* _SYS_VFS_H */
