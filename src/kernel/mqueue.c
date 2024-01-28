#include "kernel/mqueue.h"

#include <kernel/errno.h>
#include <kernel/sys/types.h>
#include <kernel/string.h>
#include <kernel/task.h>
#include <stdbool.h>
#include <stddef.h>

#define MQ_MAX_OPEN        8
#define MQ_MAX_SUBSCRIBERS 8
#define MQ_MAX_MSG_SIZE    128
#define MQ_MAX_MSG         8

struct mqueue {
    int     id;                             /* Queue ID */
    size_t  hash;                           /* Hashed name */
    pid_t   sub_ids[MQ_MAX_SUBSCRIBERS];    /* Subscribed process IDs */
    char    sub_flags[MQ_MAX_SUBSCRIBERS];  /* Subscibed processes' access flags */
    mq_attr attr;                           /* Queue attributes */
    char*   head;                           /* Message head */
    char*   tail;                           /* Message tail */
    char    messages[MQ_MAX_MSG_SIZE];      /* Queue messages */
};

struct mqueue open_queues[MQ_MAX_OPEN];
signed int mqueue_count = 0;

/*
* Find open mqueue by _hash_
*/
static struct mqueue* _mqueue_find_hash(size_t hash) {
    for (size_t i = 0; i < MQ_MAX_OPEN; i++) {
        if (open_queues[i].id && open_queues[i].hash == hash)
            return &open_queues[i];
    }

    return NULL;
}

/*
* Subscribe _pid_ to _mqueue_ with access _flags_
*/
static bool _mqueue_subscribe(struct mqueue* mqueue, pid_t pid, int flags) {
    for (size_t i = 0; i < MQ_MAX_SUBSCRIBERS; i++) {
        if (!mqueue->sub_ids[i]) {
            mqueue->sub_ids[i] = pid;
            mqueue->sub_flags[i] = flags;
            return true;
        }
    }

    return false;
};

int mqueue_open(const char *name, int oflag, __attribute__((unused)) mode_t mode,
                const mq_attr *attr) {
    size_t hash = strhash(name);
    struct mqueue *mqueue = _mqueue_find_hash(hash);

    if (oflag & O_CREAT) {
        if (oflag & O_EXCL && mqueue) {
            return -EEXIST;
        }

        if (!mqueue) {
            /* Find space */
            for (size_t i = 0; i < MQ_MAX_OPEN; i++) {
                if (!open_queues[i].id) {
                    mqueue = &open_queues[i];
                    break;
                }
            }

            if (!mqueue)
                return -ENFILE;

            if (attr) {
                if (!attr->mq_maxmsg)
                    return -EINVAL;
                if (attr->mq_maxmsg > MQ_MAX_MSG)
                    return -ENOSPC;
                if (!attr->mq_msgsize)
                    return -EINVAL;
                if (attr->mq_msgsize > MQ_MAX_MSG_SIZE)
                    return -ENOSPC;

                mqueue->attr.mq_maxmsg = attr->mq_maxmsg;
                mqueue->attr.mq_msgsize = attr->mq_msgsize;
            } else {
                mqueue->attr.mq_maxmsg = MQ_MAX_MSG;
                mqueue->attr.mq_msgsize = MQ_MAX_MSG_SIZE;
            }

            mqueue->id = ++mqueue_count;
            mqueue->hash = hash;
            mqueue->head = mqueue->messages;
            mqueue->tail = mqueue->messages;
            mqueue->attr.mq_flags = 0;
            mqueue->attr.mq_curmsg = 0;
        }
    }

    if (!mqueue)
        return -ENOENT;

    if(!_mqueue_subscribe(mqueue, task_current_pid(), oflag & 7))
        return -EACCES;

    return mqueue->id;
}
