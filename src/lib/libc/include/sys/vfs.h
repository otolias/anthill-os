/*
* VFS message handling functions and macros
*/
#ifndef _SYS_VFS_H
#define _SYS_VFS_H

#include <fcall.h>
#include <mqueue.h>

#define VFS_MAX_MSG_LEN 128

struct vfs_msg {
    fcall fcall; /* Fcall struct */
    mqd_t mq_id; /* Message queue to respond */
};

unsigned vfs_msg_parse(struct vfs_msg *msg, char* buf);

unsigned vfs_msg_put(struct vfs_msg *msg, char *buf);

#endif /* _SYS_VFS_H */
