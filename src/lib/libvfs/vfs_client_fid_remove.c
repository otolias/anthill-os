#include "vfs/client.h"

#include <stddef.h>
#include <stdlib.h>

int vfs_client_fid_remove(const struct vfs_client *client, unsigned fid) {
    struct fid_pair *pair = client->fids, *prev_pair = client->fids;

    while(pair) {
        if (pair->fid == fid)
            break;

        prev_pair = pair;
        pair = pair->next;
    }

    if (!pair) return -1;

    prev_pair->next = pair->next;
    free(pair->obj); pair->obj = NULL;
    free(pair);

    return 0;
}
