#include "vfs/client.h"

#include <stddef.h>
#include <stdlib.h>

struct vfs_client* vfs_client_remove(struct vfs_client *client) {
    struct fid_pair *pair = client->fids;

    while(pair) {
        vfs_client_remove_fid(client, pair->fid);
        pair = pair->next;
    }

    free(client);

    return NULL;
}
