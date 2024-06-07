#include "vfs/client.h"

#include <errno.h>
#include <stddef.h>
#include <stdlib.h>

struct vfs_client *first_client;

struct vfs_client* vfs_client_create(char *channel) {
    struct vfs_client *client = calloc(1, sizeof *client);
    if (!client) return NULL;

    errno = 0;
    mqd_t mq = mq_open(channel, O_WRONLY);
    if (mq == -1)
        { free(client); return NULL; }

    client->mq_id = mq;

    if (first_client) {
        struct vfs_client *last_client = first_client;

        while (last_client->next)
            last_client = last_client->next;

        last_client->next = client;
    } else {
        first_client = client;
    }

    return client;
}
