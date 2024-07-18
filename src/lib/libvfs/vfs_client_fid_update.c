#include "vfs/client.h"

int vfs_client_fid_update(const struct vfs_client *client, unsigned fid, void *obj) {
    struct fid_pair *fid_pair = client->fids;

    while (fid_pair) {
        if (fid_pair->fid == fid) break;

        fid_pair = fid_pair->next;
    }

    if (!fid_pair) return -1;

    fid_pair->obj = obj;
    return 0;
}
