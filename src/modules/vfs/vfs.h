#ifndef _MOD_VFS_H
#define _MOD_VFS_H

#include <fcall.h>
#include <mqueue.h>
#include <pstring.h>
#include <sys/vfs.h>

enum vfs_error {
    VFS_OK,
    VFS_NOMEM,
    VFS_NOTFOUND,
    VFS_MQERR,
};

struct vnode {
    pstring*         name;        /* File name */
    struct qid       qid;         /* File information */
    struct vnode*    children;    /* Array of children */
    unsigned         children_no; /* Number of children */
    struct vnode*    mount_node;  /* Parent mount point */
    mqd_t            mq_id;       /* Associated mqueue channel */
    unsigned         fid;         /* Associated mount fid */
};

/*
* Search in children of _vnode_ for vnode with name _name_
*
* On success, returns a pointer to the child.
* On failure, returns a null pointer
*/
struct vnode* vnode_find_child(const struct vnode* vnode, const pstring *name);

/*
* Get vfs starting point
*/
struct vnode* vnode_get_root();

/*
* Forwards vfs_msg _buf_ to _vnode_
*/
enum vfs_error vnode_forward(const struct vnode *vnode, char *buf);

/*
* Deallocate memory of _node_ and its children
*
* Returns null pointer
*/
[[nodiscard]] struct vnode* vnode_remove(struct vnode *node);

/*
* Descend from the root vnode each of the _n_ elements, starting from the element
* pointed to by _pstr_ and put their respective qid in the qid array pointed to by _qid_
*
* Returns the number of succesfully traversed elements and, if all elements are traversed,
* puts the last element's associated vnode to _node_
*/
unsigned short vnode_scan(pstring *elements, unsigned short n, struct qid *wqid,
                          struct vnode **node);

/*
* Setup root node. _vfs_mq_ is the input mqueue channel
* for the vfs
*/
enum vfs_error vfs_init();

#endif /* _MOD_VFS_H */
