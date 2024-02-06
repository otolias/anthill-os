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

struct mq_message {
    size_t size;                     /* Message size */
    char   message[MQ_MAX_MSG_SIZE]; /* Message content */
};

struct mq_task {
    pid_t pid;     /* Associated task pid */
    int   flags;   /* Associated task access flags */
};

struct mqueue {
    mqd_t             id;                       /* Queue ID */
    bool              open;                     /* If queue accepts connections */
    size_t            hash;                     /* Hashed name */
    struct mq_task    subs[MQ_MAX_SUBSCRIBERS]; /* Associated task pids and flags */
    struct mq_attr    attr;                     /* Queue attributes */
    int               head;                     /* Message head */
    int               tail;                     /* Message tail */
    struct mq_message messages[MQ_MAX_MSG];     /* Queue messages */
};

struct mqueue open_queues[MQ_MAX_OPEN];
size_t mqueue_count = 0;

/*
* Find open mqueue by _id_
*/
static struct mqueue* _mqueue_find(mqd_t id) {
    for (size_t i = 0; i < MQ_MAX_OPEN; i++) {
        if (open_queues[i].id == id)
            return &open_queues[i];
    }

    return NULL;
}

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
* Find _mqueue_ subscriber with _pid_. If _pid_ is 0, returns
* any subscriber
*/
static struct mq_task* _mqueue_find_subscriber(struct mqueue* mqueue, pid_t pid) {
    for (size_t i = 0; i < MQ_MAX_SUBSCRIBERS; i++) {
        if ((pid && mqueue->subs[i].pid == pid) || (!pid && mqueue->subs[i].pid > 0))
            return &mqueue->subs[i];
    }

    return NULL;
}

/*
* Unblock tasks subscribed to _mqueue_.
*
* _flag_ values:
* - BLCK_RECV for blocked tasks waiting to receive
* - BLCK_SEND for blocked tasks waiting to send
*/
static void _mqueue_unblock_subscribers(struct mqueue *mqueue, int flag) {
    for (size_t i = 0; i < MQ_MAX_SUBSCRIBERS; i++) {
        if (mqueue->subs[i].flags & flag) {
            mqueue->subs[i].flags ^= flag;
            task_unblock(mqueue->subs[i].pid);
        }
    }
}

/*
* Subscribe _pid_ to _mqueue_ with access _flags_
*/
static bool _mqueue_subscribe(struct mqueue* mqueue, pid_t pid, int flags) {
    for (size_t i = 0; i < MQ_MAX_SUBSCRIBERS; i++) {
        if (!mqueue->subs[i].pid) {
            mqueue->subs[i].pid = pid;
            mqueue->subs[i].flags = flags;
            return true;
        }
    }

    return false;
};

/*
* Unsubscribe _pid_ from _mqueue_
*/
static bool _mqueue_unsubscribe(struct mqueue* mqueue, pid_t pid) {
    struct mq_task *mq_task = _mqueue_find_subscriber(mqueue, pid);

    if (!mq_task)
        return false;

    mq_task->pid = -1;
    mq_task->flags = 0;

    return true;
}

/*
* Enqueue message pointed to by _msg_ptr_ with length _msg_len_ in
* _mqueue_
*/
static bool _mqueue_enqueue(struct mqueue* mqueue, const char *msg_ptr, size_t msg_len) {
    if (mqueue->tail == mqueue->attr.mq_maxmsg)
        mqueue->tail = -1;

    if (mqueue->attr.mq_curmsg == mqueue->attr.mq_maxmsg)
        return false;

    struct mq_message *mq_message = &mqueue->messages[++mqueue->tail];
    mq_message->size = msg_len;
    memcpy(&mq_message->message, msg_ptr, msg_len);

    /* Unblock any task that is waiting for a message */
    if (mqueue->attr.mq_curmsg == 0)
        _mqueue_unblock_subscribers(mqueue, BLCK_RECV);

    mqueue->attr.mq_curmsg++;

    return true;
}

/*
* Copy first message of _mqueue_ to _msg_ptr_
*/
static ssize_t _mqueue_dequeue(struct mqueue *mqueue, char *msg_ptr) {
    if (mqueue->head == mqueue->attr.mq_maxmsg)
        mqueue->head = -1;

    if (!mqueue->attr.mq_curmsg)
        return 0;

    const struct mq_message *mq_message = &mqueue->messages[++mqueue->head];

    memcpy(msg_ptr, &mq_message->message, mq_message->size);

    mqueue->attr.mq_curmsg--;

    return mq_message->size;
}

/*
* Destroy _mqueue_
*/
static void _mqueue_destroy(struct mqueue* mqueue) {
    mqueue->open = false;

    /* Check if there are pending subscribed processes */
    if (_mqueue_find_subscriber(mqueue, 0))
        return;

    mqueue->id = 0;
    mqueue->hash = 0;
}

int mqueue_open(const char *name, int oflag, __attribute__((unused)) mode_t mode,
                const struct mq_attr *attr) {
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
            mqueue->open = true;
            mqueue->hash = hash;
            mqueue->head = -1;
            mqueue->tail = -1;
            mqueue->attr.mq_flags = 0;
            mqueue->attr.mq_curmsg = 0;
        }
    }

    if (!mqueue || !mqueue->open)
        return -ENOENT;

    if(!_mqueue_subscribe(mqueue, task_current_pid(), oflag))
        return -EACCES;

    return mqueue->id;
}

int mqueue_close(mqd_t mqdes) {
    struct mqueue *mqueue = _mqueue_find(mqdes);

    if (!mqueue)
        return -EBADF;

    _mqueue_unsubscribe(mqueue, task_current_pid());

    if (!mqueue->open) {

        /* Check if this is the last pending connection */
        if (!_mqueue_find_subscriber(mqueue, 0))
            _mqueue_destroy(mqueue);
    }

    return 0;
}

int mqueue_unlink(const char *name) {
    struct mqueue *mqueue = _mqueue_find_hash(strhash(name));

    if (!mqueue)
        return -ENOENT;

    _mqueue_destroy(mqueue);

    return 0;
}

int mqueue_send(mqd_t id, const char *msg_ptr, size_t msg_len,
                __attribute__((unused)) unsigned msg_prio) {
    struct mqueue *mqueue = _mqueue_find(id);

    if (!mqueue)
        return -EBADF;

    if ((long) msg_len > mqueue->attr.mq_msgsize)
        return -EMSGSIZE;

    struct mq_task *mq_task = _mqueue_find_subscriber(mqueue, task_current_pid());

    if (!mq_task || mq_task->flags & O_RDONLY)
        return -EACCES;

    while(1) {
        if(_mqueue_enqueue(mqueue, msg_ptr, msg_len))
            break;

        if (mq_task->flags & O_NONBLOCK) {
            return -EAGAIN;
        }

        mq_task->flags |= BLCK_SEND;
        task_current_block();
    }

    return 0;
}

ssize_t mqueue_receive(mqd_t id, char *msg_ptr, size_t msg_len,
                       __attribute__((unused)) unsigned *msg_prio) {
    struct mqueue *mqueue = _mqueue_find(id);

    if (!mqueue)
        return -EBADF;

    if ((long) msg_len < mqueue->attr.mq_msgsize)
        return -EMSGSIZE;

    struct mq_task *mq_task = _mqueue_find_subscriber(mqueue, task_current_pid());

    if (!mq_task || mq_task->flags & O_WRONLY)
        return -EACCES;

    ssize_t result;

    while(1) {
        result = _mqueue_dequeue(mqueue, msg_ptr);
        if (result)
            break;

        if (mq_task->flags & O_NONBLOCK) {
            return -EAGAIN;
        }

        mq_task->flags |= BLCK_RECV;
        task_current_block();
    }

    return result;
}
