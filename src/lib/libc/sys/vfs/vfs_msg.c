#include "sys/vfs.h"

#include <fcall.h>
#include <mqueue.h>
#include <string.h>

#define MQ_ID_SIZE sizeof(mqd_t)

unsigned vfs_msg_parse(struct vfs_msg *msg, char *buf) {
    unsigned len = fcall_buf_to_msg((unsigned char *) buf, &msg->fcall);

    if (len == 0 || len > VFS_MAX_MSG_LEN - MQ_ID_SIZE) {
        msg->mq_id = -1;
        return 0;
    }

    msg->mq_id = *(buf + len);
    return len + MQ_ID_SIZE;
}

unsigned vfs_msg_put(struct vfs_msg *msg, char *buf) {
    unsigned len = fcall_msg_to_buf(&msg->fcall, (unsigned char *) buf,
                                    VFS_MAX_MSG_LEN - MQ_ID_SIZE);

    if (len == 0)
        return 0;

    memcpy(buf + len, &msg->mq_id, MQ_ID_SIZE);

    return len + MQ_ID_SIZE;
}

unsigned vfs_msg_send(struct vfs_msg *msg, char *buf, mqd_t out, mqd_t in) {
    unsigned len;

    len = vfs_msg_put(msg, buf);
    if (len == 0) return 0;

    if (mq_send(out, buf, len, 0) == -1)
        return 0;

    if (mq_receive(in, buf, VFS_MAX_MSG_LEN, 0) == -1)
        return 0;

    len = vfs_msg_parse(msg, buf);
    if (len == 0)
        return 0;

    return len;
}
