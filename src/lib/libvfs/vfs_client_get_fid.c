#include "vfs/client.h"

#include <stddef.h>

void* vfs_client_get_fid(const struct vfs_client *client, unsigned fid) {
    struct fid_pair *fid_pair = client->fids;

    while (fid_pair) {
        if (fid_pair->fid == fid)
            return fid_pair->obj;

        fid_pair = fid_pair->next;
    }

    return NULL;
}
