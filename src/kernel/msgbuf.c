#include "kernel/msgbuf.h"

#include <kernel/arch/mem.h>
#include <kernel/panic.h>
#include <kernel/string.h>
#include <kernel/task.h>

/* Number of message buffers in linked node */
#define NODE_BUF_CNT ((PAGESIZE - sizeof(struct msgbuf_node *)) / sizeof(struct msgbuf))
/* Receive queue size */
#define QUEUE_SIZE    8
/* Maximum message buffer connections */
#define MAX_CONN      8

struct conn {
    msgbuf_id id; /* Receiver ID */
    char* addr;   /* Address of buffer in receiver's process image */
};

struct msgbuf_r_conn {
    struct conn* conn;
    enum kern_err err;
};

struct msgbuf {
    char name[__MSB_NAME_MAX];         /* Message buffer name */
    char* addr;                        /* Message buffer user space address */
    struct task* owner;                /* Message buffer owner */
    size_t size;                       /* Message buffer size */
    msgbuf_id recv_queue[QUEUE_SIZE];  /* Message buffer receive queue */
    size_t recv_queue_pos;             /* Message buffer receive queue position */
    struct conn connections[MAX_CONN]; /* Message buffer connection */
};

struct msgbuf_node {
    struct msgbuf_node* next;            /* Pointer to next node */
    struct msgbuf buffers[NODE_BUF_CNT]; /* Node's message buffers */
};

static struct msgbuf_node *first = NULL;

/*
* Allocate _size_ bytes and map them to current process' image.
*
* Returns mem_r_addr:
* - On success, _addr_ is a pointer to the virtual address of the allocated
*   space and _err_ is set to ERR_OK.
* - On failure, _addr_ is NULL and _err_ is set to indicate the error.
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
            MEM_RW,
            MEM_NORM
        );
        if (err != ERR_OK) {
            // TODO: Free previous
            return (struct mem_r_addr) { .addr = NULL, .err = page_r.err };
        }
    }

    return (struct mem_r_addr) { .addr = vaddr, .err = ERR_OK };
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

    struct msgbuf_node *node = first;
    msgbuf_id id = -1;
    struct msgbuf *new_msgbuf = NULL;

    for (size_t list_idx = 0; ; list_idx++) {
        for (size_t buffer_idx = 0; buffer_idx < NODE_BUF_CNT; buffer_idx++) {
            struct msgbuf *msgbuf = &node->buffers[buffer_idx];

            // Check for name collisions
            if (strncmp(name, msgbuf->name, __MSB_NAME_MAX) == 0)
                return (struct msgbuf_r_id) { .id = -1, .err = ERR_MSB_EXS };

            // Keep index of first empty message buffer
            if (id == -1 && strlen(msgbuf->name) == 0) {
                id = (msgbuf_id) (list_idx * NODE_BUF_CNT + buffer_idx);
                new_msgbuf = msgbuf;
            }
        }

        struct msgbuf_node *next = node->next;
        if (!next && id == -1) {
            struct mem_r_addr addr_r = mem_kernel_alloc_page(MEM_RW);
            if (addr_r.err != ERR_OK)
                return (struct msgbuf_r_id) { .id = -1, .err = addr_r.err };

            next = addr_r.addr;
            node->next = next;
        }

        if (next) {
            node = next;
            continue;
        }

        break;
    }

    if (strlcpy(new_msgbuf->name, name, __MSB_NAME_MAX) > __MSB_NAME_MAX) {
        *new_msgbuf->name = 0;
        return (struct msgbuf_r_id) { .id = -1, .err = ERR_MSB_NAM };
    }

    return (struct msgbuf_r_id) { .id = id, .err = ERR_OK };
}

/*
* Get message buffer identified by _id_.
*
* Returns a pointer to the message buffer.
*/
static struct msgbuf* _msgbuf_get(msgbuf_id id) {
    size_t list_idx = id / NODE_BUF_CNT;
    size_t buffer_idx = id % NODE_BUF_CNT;

    struct msgbuf_node *node = first;
    while (list_idx--) {
        if (!node->next)
            return NULL;

        node = node->next;
    }

    struct msgbuf *msgbuf = &node->buffers[buffer_idx];
    if (*msgbuf->name == 0)
        return NULL;

    return msgbuf;
}

/*
* Get the ID of message buffer identified by _name_.
*
* On success, returns the message buffer's ID.
* On failure, returns -1.
*/
static msgbuf_id _msgbuf_get_id(const char * const name) {
    struct msgbuf_node *node = first;

    for (size_t list_idx = 0; ; list_idx++) {
        for (size_t buffer_idx = 0; buffer_idx < NODE_BUF_CNT; buffer_idx++) {
            struct msgbuf *msgbuf = &node->buffers[buffer_idx];

            if (strncmp(msgbuf->name, name, __MSB_NAME_MAX) == 0)
                return (msgbuf_id) (list_idx * NODE_BUF_CNT + buffer_idx);
        }

        if (node->next)
            node = node->next;

        return -1;
    }
}

/*
* Get next sender ID from _msgbuf_ receive queue and advance queue position.
*
* On success, returns the sender's ID.
* On failure, returns -1.
*/
static msgbuf_id _msgbuf_queue_pop(struct msgbuf *msgbuf) {
    const size_t pos = msgbuf->recv_queue_pos;
    msgbuf_id id = msgbuf->recv_queue[pos];
    if (id == -1)
        return -1;

    msgbuf->recv_queue[pos] = -1;
    msgbuf->recv_queue_pos = (pos + 1) % QUEUE_SIZE;
    return id;
}

/*
* Push _send_id_ to the receive queue of _msgbuf_, if not full.
*
* On success, returns 0.
* On failure, returns -1.
*/
static int _msgbuf_queue_push(struct msgbuf *msgbuf, msgbuf_id send_id) {
    for (size_t i = msgbuf->recv_queue_pos; i < QUEUE_SIZE + msgbuf->recv_queue_pos; i++) {
        if (msgbuf->recv_queue[i % QUEUE_SIZE] == -1) {
            msgbuf->recv_queue[i % QUEUE_SIZE] = send_id;
            return 0;
        }
    }

    return -1;
}

/*
* Create a connection between message buffer pointed to by _msgbuf_ and message
* buffer associated with _recv_id_. Fail if connection already exists.
*
* Returns msgbuf_r_conn:
* - On success, _conn_ is the new connection and _err_ is set to ERR_OK.
* - On failure, _conn_ is NULL and _err_ is set to indicate the error.
*/
static struct msgbuf_r_conn _conn_alloc(struct msgbuf *msgbuf, msgbuf_id recv_id) {
    struct conn *conn = NULL;
    for (size_t i = 0; i < MAX_CONN; i++) {
        if (!conn && msgbuf->connections[i].id == -1)
            conn = &msgbuf->connections[i];

        if (msgbuf->connections[i].id == recv_id)
            return (struct msgbuf_r_conn) { .conn = NULL, .err = ERR_MSB_COE };
    }

    if (!conn)
        return (struct msgbuf_r_conn) { .conn = NULL, .err = ERR_MSB_COF };

    return (struct msgbuf_r_conn) { .conn = conn, .err = ERR_OK };
}

/*
* Destroy connection between _msgbuf_ and _recv_id_ and unmap underlying buffer.
*
* Returns kern_err.
*/
static enum kern_err _conn_free(struct msgbuf *msgbuf, msgbuf_id recv_id) {
    struct conn *conn;

    for (size_t i = 0; i < MAX_CONN; i++) {
        conn = &msgbuf->connections[i];

        if (conn->id == recv_id)
            break;
    }

    if (!conn)
        return ERR_MSB_COD;

    const struct msgbuf *receiver = _msgbuf_get(conn->id);
    for (size_t p = 0; p < MEM_SIZE_TO_PAGES(msgbuf->size); p++) {
        struct mem_r_addr addr_r = mem_user_unmap_page(
            MEM_PHYS_TO_VIRT(receiver->owner->tran_table),
            conn->addr + (p * PAGESIZE)
        );
        if (addr_r.err != ERR_OK)
            panic("Message buffer disconnection tried to unmap non-mapped page");
    }

    conn->id = -1;
    conn->addr = NULL;

    return ERR_OK;
}

/*
* Get _msgbuf_ connection with _recv_id_.
*
* On success, returns a pointer to the connection.
* On failure, returns NULL.
*/
static struct conn* _conn_get(struct msgbuf *msgbuf, msgbuf_id recv_id) {
    for (size_t i = 0; i < MAX_CONN; i++) {
        if (msgbuf->connections[i].id == recv_id)
            return &msgbuf->connections[i];
    }

    return NULL;
}

enum kern_err msgbuf_close(msgbuf_id id) {
    struct msgbuf *msgbuf = _msgbuf_get(id);
    if (!msgbuf || msgbuf->owner->pid != task_current()->pid)
        return ERR_MSB_FND;

    if (msgbuf->addr) {
        // Disconnect all connections
        for (size_t c = 0; c < MAX_CONN; c++) {
            struct conn *conn = &msgbuf->connections[c];
            if (conn->id == -1)
                continue;

            // TODO: Fix double array search
            _conn_free(msgbuf, conn->id);
        }

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

    // TODO: De-allocate space from node if needed

    return ERR_OK;
}

struct msgbuf_r_id msgbuf_connect(msgbuf_id id, const char *name) {
    struct msgbuf *sender = _msgbuf_get(id);
    if (!sender)
        return (struct msgbuf_r_id) { .id = -1, .err = ERR_MSB_FND };

    const msgbuf_id receiver_id = _msgbuf_get_id(name);
    if (receiver_id == -1)
        return (struct msgbuf_r_id) { .id = -1, .err = ERR_MSB_FND };

    struct msgbuf *receiver = _msgbuf_get(receiver_id);
    if (!receiver)
        panic("Message buffer ID does not correspond to any message buffer");

    // Create connection
    struct msgbuf_r_conn conn_r = _conn_alloc(sender, receiver_id);
    if (conn_r.err != ERR_OK)
        return (struct msgbuf_r_id) { .id = -1, .err = conn_r.err };

    // Map sender's buffer to receiver owner's process space
    void *send_tran_table = MEM_PHYS_TO_VIRT(sender->owner->tran_table);
    void *recv_tran_table = MEM_PHYS_TO_VIRT(receiver->owner->tran_table);
    size_t page_cnt = MEM_SIZE_TO_PAGES(sender->size);
    void *recv_buf = mem_user_find_empty(recv_tran_table, (void *) 0x200000, page_cnt);
    if (!recv_buf)
        return (struct msgbuf_r_id) { .id = -1, .err = ERR_MEM_OOM };

    void *mapped[page_cnt];
    for (size_t i = 0; i < page_cnt; i++) {
        mapped[i] = recv_buf + (i * PAGESIZE);
        void *paddr = MEM_VIRT_TO_PHYS(mem_user_get_kaddr(send_tran_table, sender->addr));
        enum kern_err err = mem_user_map_page(
            recv_tran_table,
            mapped[i],
            paddr,
            MEM_RW,
            MEM_NORM
        );
        if (err != ERR_OK) {
            for (size_t j = 0; j < i; j++)
                mem_user_unmap_page(recv_tran_table, mapped[j]);

            return (struct msgbuf_r_id) { .id = -1, .err = err };
        }
    }

    conn_r.conn->id = receiver_id;
    conn_r.conn->addr = recv_buf;

    return (struct msgbuf_r_id) { .id = receiver_id, .err = ERR_OK };
}

struct msgbuf_r_id msgbuf_create(const char *name, size_t size, char **send_buf) {
    const size_t name_len = strlen(name);
    if (name_len == 0 || name_len > __MSB_NAME_MAX)
        return (struct msgbuf_r_id) { .id = -1, .err = ERR_MSB_NAM };

    if (send_buf && size == 0)
        return (struct msgbuf_r_id) { .id = -1, .err = ERR_MSB_SIZ };

    // Allocate new message buffer
    struct msgbuf_r_id id_r = _msgbuf_alloc(name);
    if (id_r.err != ERR_OK)
        return (struct msgbuf_r_id) { .id = -1, .err = id_r.err };

    struct msgbuf *msgbuf = _msgbuf_get(id_r.id);
    msgbuf->owner = task_current();

    for (size_t p = 0; p < QUEUE_SIZE; p++)
        msgbuf->recv_queue[p] = -1;

    for (size_t i = 0; i < MAX_CONN; i++) {
        msgbuf->connections[i].id = -1;
        msgbuf->connections[i].addr = NULL;
    }

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

enum kern_err msgbuf_disconnect(msgbuf_id send_id, msgbuf_id recv_id) {
    struct msgbuf *sender = _msgbuf_get(send_id);
    struct msgbuf *receiver = _msgbuf_get(recv_id);
    if (!sender || sender->owner->pid != task_current()->pid || !receiver)
        return ERR_MSB_FND;

    return _conn_free(sender, recv_id);
}

struct msgbuf_r_id msgbuf_recv(msgbuf_id id, char **recv_buf) {
    struct msgbuf *receiver = _msgbuf_get(id);
    if (!receiver || receiver->owner->pid != task_current()->pid)
        return (struct msgbuf_r_id) { .id = -1, .err = ERR_MSB_FND };

    msgbuf_id send_id;
    struct conn *conn;
    while (1) {
        send_id = _msgbuf_queue_pop(receiver);
        if (send_id == -1) {
            task_block(task_current()->pid, TASK_BLOCKED);
            continue;
        }

        struct msgbuf *sender = _msgbuf_get(send_id);
        if (!sender)
            continue;

        conn = _conn_get(sender, id);
        if (!conn)
            continue;

        break;
    }

    *recv_buf = conn->addr;
    return (struct msgbuf_r_id) { .id = send_id, .err = ERR_OK };
}

enum kern_err msgbuf_reply(msgbuf_id id, msgbuf_id reply_id) {
    struct msgbuf *sender = _msgbuf_get(id);
    struct msgbuf *receiver = _msgbuf_get(reply_id);
    if (!sender || sender->owner->pid != task_current()->pid || !receiver)
        return ERR_MSB_FND;

    task_unblock(receiver->owner->pid, TASK_BLOCKED);

    return ERR_OK;
}

enum kern_err msgbuf_send(msgbuf_id send_id, msgbuf_id recv_id, enum msgbuf_send_flags flags) {
    struct msgbuf *sender = _msgbuf_get(send_id);
    struct msgbuf *receiver = _msgbuf_get(recv_id);
    if (!sender || sender->owner->pid != task_current()->pid || !receiver)
        return ERR_MSB_FND;

    if (!_conn_get(sender, recv_id))
        return ERR_MSB_COD;

    // Append to receiver's queue. If it fails, block and retry
    while (_msgbuf_queue_push(receiver, send_id) == -1)
        task_block(task_current()->pid, TASK_BLOCKED);

    // Unblock receiver
    task_unblock(receiver->owner->pid, TASK_BLOCKED);

    if (flags & MSB_NOBLOCK)
        return ERR_OK;

    // Block until a reply has been sent
    task_block(task_current()->pid, TASK_BLOCKED);

    return ERR_OK;
}
