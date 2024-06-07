#include "vfs/client.h"

#include <stddef.h>
#include <stdlib.h>

struct fid_pair* vfs_client_add_fid(struct vfs_client *client, unsigned fid, void *obj) {
    struct fid_pair *fid_pair = calloc(1, sizeof *fid_pair);
    if (!fid_pair) return NULL;

    fid_pair->fid = fid;
    fid_pair->obj = obj;

    struct fid_pair *prev_pair = client->fids;
    if (prev_pair) {
        while (prev_pair->next)
            prev_pair = prev_pair->next;

        prev_pair->next = fid_pair;
    } else
        client->fids = fid_pair;

    return fid_pair;
}
