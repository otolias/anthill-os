#include "client.h"

#include <errno.h>
#include <fcall.h>
#include <mqueue.h>
#include <pstring.h>
#include <stddef.h>
#include <stdlib.h>

#include "vfs.h"

struct client *first_client;

struct client* client_create(pstring *channel) {
    char channel_buf[16];
    if (!pstrtoz(channel_buf, channel, 16)) return NULL;

    errno = 0;
    mqd_t mq = mq_open(channel_buf, O_WRONLY);
    if (mq == -1) return NULL;

    struct client *client = malloc(sizeof *client);
    if (!client) return NULL;

    client->mq_id = mq;

    for (int i = 0; i < FOPEN_MAX; i++) {
        client->fids[i].fid = NOFID;
        client->fids[i].node = NULL;
    }

    if (first_client) {
        struct client *last_client = first_client;

        while (last_client->next)
            last_client = last_client->next;

        last_client->next = client;
    } else {
        first_client = client;
    }

    return client;
}

void client_delete(struct client *client) {
    const struct client *prev = client->prev;

    if (prev) client->prev->next = client->next;
    else first_client = client->next;

    free(client);
}

struct client* client_get(mqd_t mq_id) {
    struct client *client = first_client;

    while (client->mq_id != mq_id)
        client = client->next;

    return client;
}

struct vnode* client_get_node(struct client *client, unsigned fid) {
    unsigned i;
    for (i = 0; i < FOPEN_MAX; i++) {
        if (client->fids[i].fid == fid)
            return client->fids[i].node;
    }

    return NULL;
}

enum vfs_error client_open(struct client *client, unsigned fid, struct vnode *vnode) {
    unsigned i = 0;

    for (i = 0; i < FOPEN_MAX; i++) {
        if (client->fids[i].fid == NOFID)
            break;
    }

    if (i == FOPEN_MAX) return VFS_NOMEM;

    client->fids[i].fid = fid;
    client->fids[i].node = vnode;

    return VFS_OK;
}
