#ifndef _LIBVFS_CLIENT_H
#define _LIBVFS_CLIENT_H

#include <mqueue.h>

/*
* A general container for storing data associated with an fid
*/
struct fid_pair {
    unsigned         fid;  /* Associated fid */
    void*            obj;  /* Pointer to container */
    struct fid_pair* next; /* Pointer to next fid_obj */
};

struct vfs_client {
    mqd_t              mq_id; /* Associated mqueue channel */
    struct fid_pair*   fids;  /* Pointer to first associated fid object */
    struct vfs_client* prev;  /* Pointer to next client */
    struct vfs_client* next;  /* Pointer to previous client */
};

extern struct vfs_client *first_client;

/*
* Create a new _fid_ and _obj_ association for _client_
*
* On success, returns the new association.
* On failure, returns a null pointer.
*/
struct fid_pair* vfs_client_add_fid(struct vfs_client *client, unsigned fid, void *obj);

/*
* Create new client on mqueue _channel_, and associate _obj_ with it
*
* On success, returns the new vfs_client.
* On failure, returns a null pointer.
*/
struct vfs_client* vfs_client_create(char *channel);

/*
* Returns client associated with mqueue channel _mq_id_.
*
* On success, returns the associated client.
* On failure, returns a null pointer.
*/
struct vfs_client* vfs_client_get(mqd_t mq_id);

/*
* Returns a pointer to the object associated with _fid_ of _client_.
*
* On success, returns a pointer to the corresponding object.
* On failure, returns a null pointer.
*/
void* vfs_client_get_fid(const struct vfs_client *client, unsigned fid);

/*
* Removes _client_. Returns a null pointer.
*/
[[nodiscard]] struct vfs_client* vfs_client_remove(struct vfs_client *client);

/*
* Removes fid association of _client_ and _fid_.
*
* On success, returns 0.
* On failure, returns -1.
*/
int vfs_client_remove_fid(const struct vfs_client *client, unsigned fid);


#endif /* _LIBVFS_CLIENT_H */
