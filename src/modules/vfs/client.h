#ifndef _MOD_VFS_CLIENT_H
#define _MOD_VFS_CLIENT_H

#include <mqueue.h>
#include <pstring.h>
#include <stdio.h>

#include "vfs.h"

struct fid_pair {
    unsigned      fid;
    struct vnode* node;
};

struct client {
    mqd_t           mq_id;           /* Associated mqueue channel */
    struct fid_pair fids[FOPEN_MAX]; /* Associated fids */
    struct client*  prev;            /* Previous client struct */
    struct client*  next;            /* Next client struct */
};

/*
* Create new client on mqueue _channel_
*/
struct client* client_create(pstring *channel);

/*
* Deallocate _client_ resources
*/
void client_delete(struct client *client);

/*
* Get client by mqueue id
*/
struct client* client_get(mqd_t mq_id);

/*
* Get node associated with _fid_ of _client_
*/
struct vnode* client_get_node(struct client *client, unsigned fid);

/*
* Associate _client_ _fid_ with _vnode_
*/
enum vfs_error client_open(struct client *client, unsigned fid, struct vnode *vnode);

#endif /* _MOD_VFS_CLIENT_H */
