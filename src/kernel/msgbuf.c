#include "kernel/msgbuf.h"

#include <kernel/arch/mem.h>
#include <kernel/error.h>
#include <kernel/string.h>
#include <kernel/task.h>
#include <stdbool.h>

/* Number of message buffers in an arena */
#define ARENA_BUF_CNT ((PAGESIZE - sizeof(struct msgbuf_arena *)) / sizeof(struct msgbuf))
/* Receive queue size */
#define QUEUE_SIZE    8
/* Maximum message buffer channels */
#define MAX_CHANNELS  8

struct channel {
    msgbuf_id id;
    void *addr;
};

struct msgbuf {
    char name[__MSB_NAME_MAX];             /* Message buffer name */
    char *addr;                            /* Message buffer user space address */
    struct task* owner;                    /* Message buffer owner */
    size_t size;                           /* Message buffer size */
    msgbuf_id recv_queue[QUEUE_SIZE];      /* Message buffer receive queue */
    size_t recv_queue_pos;                 /* Message buffer receive queue position */
    struct channel channels[MAX_CHANNELS]; /* Message buffer connection channels */
};

struct msgbuf_arena {
    struct msgbuf_arena* next;            /* Pointer to next arena */
    struct msgbuf buffers[ARENA_BUF_CNT]; /* Arena message buffers */
};

static struct msgbuf_arena *first = NULL;

/*
* Allocate _size_ continuous bytes and map to current process' image.
*
* Returns mem_r_addr:
* - On success, _addr_ is a pointer to the virtual address of the allocated
*   space and _err_ is set to ERR_OK.
* - On failure, _addr_ is NULL and _err_ is set to to indicate the error.
*/
static struct mem_r_addr _buffer_alloc(size_t size) {
    void * const tran_table = MEM_PHYS_TO_VIRT(task_current()->tran_table);
    size_t page_cnt = MEM_SIZE_TO_PAGES(size);

    void * const vaddr = mem_user_find_empty(tran_table, (void *) 0x200000, page_cnt);
    if (!vaddr)
        return (struct mem_r_addr) { .addr = NULL, .err = ERR_MEM_OOM };

    for (size_t i = 0; i < page_cnt; i++) {
        struct mem_r_addr page_r = mem_kernel_alloc_page(MEM_RW);
        if (page_r.err != ERR_OK) {
            // TODO: Free previous
            return (struct mem_r_addr) { .addr = NULL, .err = page_r.err };
        }

        enum kern_err err = mem_user_map_page(
            tran_table,
            vaddr + (i * PAGESIZE),
            MEM_VIRT_TO_PHYS(page_r.addr),
            MEM_RW
        );
        if (err != ERR_OK) {
            // TODO: Free previous
            return (struct mem_r_addr) { .addr = NULL, .err = page_r.err };
        }
    }

    return (struct mem_r_addr) { .addr = vaddr, .err = ERR_OK };
}

/*
* Add _id_ and _addr_ to a channel of _msgbuf_, if there is no channel with
* _id_.
*
* On success, returns a pointer to the channel.
* On failure, returns NULL.
*/
static struct channel* _channel_add(struct msgbuf *msgbuf, msgbuf_id id, char *addr) {
    struct channel *new = NULL;
    for (size_t i = 0; i < MAX_CHANNELS; i++) {
        if (!new && msgbuf->channels[i].id == -1)
            new = &msgbuf->channels[i];

        if (msgbuf->channels[i].id == id)
            return &msgbuf->channels[i];
    }

    if (!new)
        return NULL;

    new->id = id;
    new->addr = addr;
    return new;
}

/*
* Get _msgbuf_ channel identified by _id_.
*
* On success, returns a pointer to the channel.
* On failure, returns NULL.
*/
static struct channel* _channel_get(struct msgbuf *msgbuf, msgbuf_id id) {
    for (size_t i = 0; i < MAX_CHANNELS; i++) {
        if (msgbuf->channels[i].id == id)
            return &msgbuf->channels[i];
    }

    return NULL;
}

/*
* Allocate a new message buffer identified by _name_. Fail if a message buffer
* identified by _name_ already exists.
*
* Returns msgbuf_r_id:
* - On success, _id_ is the message buffer ID and _err_ is set to ERR_OK.
* - On failure, _id_ is -1 and _err_ is set to indicate the error.
*/
static struct msgbuf_r_id _msgbuf_alloc(const char * const name) {
    if (!first) {
        struct mem_r_addr addr_r = mem_kernel_alloc_page(MEM_RW);
        if (addr_r.err != ERR_OK)
            return (struct msgbuf_r_id) { .id = -1, .err = addr_r.err };

        first = addr_r.addr;
        first->next = NULL;
    }

    struct msgbuf_arena *arena = first;
    msgbuf_id id = -1;
    struct msgbuf *msgbuf = NULL;

    for (size_t arena_idx = 0; ; arena_idx++) {
        for (size_t buffer_idx = 0; buffer_idx < ARENA_BUF_CNT; buffer_idx++) {
            // Check for name collisions
            if (strncmp(name, arena->buffers[buffer_idx].name, __MSB_NAME_MAX) == 0)
                return (struct msgbuf_r_id) { .id = -1, .err = ERR_MSB_EXS };

            // Keep index of first empty message buffer
            if (id == -1 && *arena->buffers[buffer_idx].name == 0) {
                id = (msgbuf_id) (arena_idx * ARENA_BUF_CNT + buffer_idx);
                msgbuf = &arena->buffers[buffer_idx];
            }
        }

        struct msgbuf_arena *next = arena->next;
        if (!next && id == -1) {
            struct mem_r_addr addr_r = mem_kernel_alloc_page(MEM_RW);
            if (addr_r.err != ERR_OK)
                return (struct msgbuf_r_id) { .id = -1, .err = addr_r.err };

            next = addr_r.addr;
            arena->next = next;
        }

        if (next) {
            arena = next;
            continue;
        }

        break;
    }

    if (strlcpy(msgbuf->name, name, __MSB_NAME_MAX) > __MSB_NAME_MAX) {
        *msgbuf->name = 0;
        return (struct msgbuf_r_id) { .id = -1, .err = ERR_MSB_LNG };
    }

    return (struct msgbuf_r_id) { .id = id, .err = ERR_OK };
}

/*
* Get message buffer identified by _id_.
*
* Returns a pointer to the message buffer.
*/
static struct msgbuf* _msgbuf_get(msgbuf_id id) {
    size_t arena_idx = id / ARENA_BUF_CNT;
    size_t buffer_idx = id % ARENA_BUF_CNT;

    struct msgbuf_arena *arena = first;
    while (arena_idx--) {
        if (!arena->next)
            return NULL;

        arena = arena->next;
    }

    struct msgbuf *msgbuf = &arena->buffers[buffer_idx];
    if (*msgbuf->name == 0)
        return NULL;

    return msgbuf;
}

/*
* Get message buffer identified by _name_.
*
* On success, returns the message buffer's ID.
* On failure, returns -1.
*/
static msgbuf_id _msgbuf_get_name(const char * const name) {
    struct msgbuf_arena *arena = first;

    for (size_t arena_idx = 0; ; arena_idx++) {
        for (size_t buffer_idx = 0; buffer_idx < ARENA_BUF_CNT; buffer_idx++) {
            struct msgbuf *msgbuf = &arena->buffers[buffer_idx];

            if (strncmp(msgbuf->name, name, __MSB_NAME_MAX) == 0)
                return (msgbuf_id) (arena_idx * ARENA_BUF_CNT + buffer_idx);
        }

        if (arena->next)
            arena = arena->next;

        return -1;
    }
}

/*
* Get next sender ID from _msgbuf_ receive queue entry advance queue position.
*
* On success, returns the sender message buffer's ID.
* On failure, returns -1.
*/
static msgbuf_id _recv_queue_pop(struct msgbuf *msgbuf) {
    for (size_t i = msgbuf->recv_queue_pos; i < QUEUE_SIZE + msgbuf->recv_queue_pos; i++) {
        if (msgbuf->recv_queue[i % QUEUE_SIZE] != -1) {
            msgbuf_id id = msgbuf->recv_queue[i % QUEUE_SIZE];
            msgbuf->recv_queue[i % QUEUE_SIZE] = -1;
            msgbuf->recv_queue_pos = (i % QUEUE_SIZE) + 1;
            return id;
        }
    }

    return -1;
}

/*
* Push _id_ to the receive queue of _msgbuf_, if not full.
*
* On success, returns _id_.
* On failure, returns -1.
*/
static msgbuf_id _recv_queue_push(struct msgbuf *msgbuf, msgbuf_id id) {
    for (size_t i = msgbuf->recv_queue_pos; i < QUEUE_SIZE + msgbuf->recv_queue_pos; i++) {
        if (msgbuf->recv_queue[i % QUEUE_SIZE] == -1) {
            msgbuf->recv_queue[i % QUEUE_SIZE] = id;
            return id;
        }
    }

    return -1;
}

enum kern_err msgbuf_close(msgbuf_id id) {
    struct msgbuf *msgbuf = _msgbuf_get(id);
    if (!msgbuf || msgbuf->owner->pid != task_current()->pid)
        return ERR_MSB_FND;

    if (msgbuf->addr) {
        // Destroy channels
        for (size_t c = 0; c < MAX_CHANNELS; c++) {
            struct channel *chan = &msgbuf->channels[c];
            if (chan->id == -1)
                continue;

            const struct msgbuf *receiver = _msgbuf_get(chan->id);

            // Unmap buffer from receiver's process image
            for (size_t p = 0; p < MEM_SIZE_TO_PAGES(msgbuf->size); p++) {
                mem_user_unmap_page(
                    MEM_PHYS_TO_VIRT(receiver->owner->tran_table),
                    chan->addr + (p * PAGESIZE)
                );
            }
        }

        // Unmap from this process' image
        void * const tran_table = MEM_PHYS_TO_VIRT(task_current()->tran_table);

        for (size_t p = 0; p < MEM_SIZE_TO_PAGES(msgbuf->size); p++) {
            void * const vaddr = msgbuf->addr + (p * PAGESIZE);
            void * const kaddr = mem_user_get_kaddr(tran_table, vaddr);

            mem_user_unmap_page(tran_table, vaddr);

            // De-allocate kernel page
            enum kern_err err = mem_kernel_free_page(kaddr);
            if (err != ERR_OK)
                return err;
        }
    }

    memset(msgbuf->name, 0, __MSB_NAME_MAX);

    // TODO: De-allocate space from arena if needed

    return ERR_OK;
}

struct msgbuf_r_id msgbuf_create(const char * const name, size_t size, char **send_buf) {
    if (send_buf && size == 0)
        return (struct msgbuf_r_id) { .id = -1, .err = ERR_MSB_SIZ };

    // Create new message buffer
    struct msgbuf_r_id id_r = _msgbuf_alloc(name);
    if (id_r.err != ERR_OK)
        return id_r;

    struct msgbuf *msgbuf = _msgbuf_get(id_r.id);
    msgbuf->owner = task_current();

    for (size_t p = 0; p < QUEUE_SIZE; p++)
        msgbuf->recv_queue[p] = -1;

    for (size_t i = 0; i < MAX_CHANNELS; i++) {
        msgbuf->channels[i].id = -1;
        msgbuf->channels[i].addr = NULL;
    }

    // Allocate and map buffer
    if (send_buf) {
        struct mem_r_addr addr_r = _buffer_alloc(size);
        if (addr_r.err != ERR_OK)
            return (struct msgbuf_r_id) { .id = -1, .err = addr_r.err };

        msgbuf->addr = addr_r.addr;
        msgbuf->size = size;
        *send_buf = addr_r.addr;
    } else {
        msgbuf->addr = NULL;
        msgbuf->size = 0;
    }

    return (struct msgbuf_r_id) { .id = id_r.id, .err = ERR_OK };
}

struct msgbuf_r_buf msgbuf_recv(msgbuf_id recv_id) {
    struct msgbuf *receiver = _msgbuf_get(recv_id);
    if (!receiver || receiver->owner->pid != task_current()->pid)
        return (struct msgbuf_r_buf) { .buf = NULL, .err = ERR_MSB_FND };

    msgbuf_id send_id;
    while (1) {
        send_id = _recv_queue_pop(receiver);
        if (send_id != -1)
            break;

        task_block(task_current()->pid, TASK_BLOCKED);
    }

    // TODO: Different error code
    // The message buffer may have been closed
    struct msgbuf *sender = _msgbuf_get(send_id);
    if (!sender)
        return (struct msgbuf_r_buf) { .buf = NULL, .err = ERR_MSB_FND };

    struct channel *sender_chan = _channel_get(sender, recv_id);
    if (!sender_chan)
        return (struct msgbuf_r_buf) { .buf = NULL, .err = ERR_MSB_CHN };

    return (struct msgbuf_r_buf) { .buf = sender_chan->addr, .err = ERR_OK };
}

enum kern_err msgbuf_send(msgbuf_id send_id, const char * const recv_name) {
    // Check if sender exists
    struct msgbuf *sender = _msgbuf_get(send_id);
    if (!sender)
        return ERR_MSB_FND;

    const msgbuf_id receiver_id = _msgbuf_get_name(recv_name);
    struct msgbuf *receiver = _msgbuf_get(receiver_id);
    if (!receiver)
        return ERR_MSB_FND;

    if (!_channel_get(sender, receiver_id)) {
        // Map sender's buffer to receiver owner's process space
        void *tran_table = MEM_PHYS_TO_VIRT(receiver->owner->tran_table);
        size_t page_cnt = MEM_SIZE_TO_PAGES(sender->size);
        void *recv_buf = mem_user_find_empty(tran_table, (void *) 0x200000, page_cnt);
        if (!recv_buf)
            return ERR_MEM_OOM;

        for (size_t i = 0; i < page_cnt; i++) {
            void *vaddr = recv_buf + (i * PAGESIZE);
            void *paddr = MEM_VIRT_TO_PHYS(mem_user_get_kaddr(tran_table, sender->addr));

            enum kern_err err = mem_user_map_page(
                tran_table,
                vaddr,
                paddr,
                MEM_RW
            );
            if (err != ERR_OK) {
                // TODO: Unmap previous
                return err;
            }
        }

        if (!_channel_add(sender, receiver_id, recv_buf)) {
            // TODO: Unmap previous
            return ERR_MSB_CHF;
        }
    }

    // Append to receiver's queue. If it fails, block and retry
    while (_recv_queue_push(receiver, send_id) == -1)
        task_block(task_current()->pid, TASK_BLOCKED);

    task_unblock(receiver->owner->pid, TASK_BLOCKED);

    return ERR_OK;
}
