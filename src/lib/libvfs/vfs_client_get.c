#include "vfs/client.h"

#include <stddef.h>

struct vfs_client* vfs_client_get(mqd_t mq_id) {
    struct vfs_client *client = first_client;

    while (client) {
        if (client->mq_id == mq_id)
            return client;

        client = client->next;
    }

    return NULL;
}
