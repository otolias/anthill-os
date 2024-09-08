#ifndef _MOD_VFS_VNODE_H
#define _MOD_VFS_VNODE_H

#include <fcall.h>
#include <mqueue.h>
#include <pstring.h>

struct vnode {
    pstring*      name;         /* File name */
    struct qid    qid;          /* File information */
    struct vnode* children;     /* Array of children */
    struct vnode* mount_node;   /* Parent mount point */
    struct vnode* channel_node; /* If remote, communication node */
    unsigned      children_no;  /* Number of children */
    mqd_t         mq_id;        /* Associated mqueue channel */
};

extern struct vnode _root_vnode;

/*
* Allocate space for a child of _vnode_
*
* On success, returns a pointer to the child.
* On failure, returns a null pointer.
*/
struct vnode* vnode_add_child(struct vnode *vnode);

/*
* Send Tattach message to _channel_ and setup file tree under _mount_.
*
* On success, returns a pointer to _mount_.
* On failure, returns a null pointer.
*/
struct vnode* vnode_attach(const struct vnode *channel, struct vnode *mount);

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
* Put up to _count_ bytes of directory entries to _buf_ represented by _vnode_ starting
* from _offset_.
*
* On success, returns the number of bytes written.
* On failure, returns 0.
*/
unsigned vnode_read_dir(const struct vnode *vnode, unsigned char *buf,
                        unsigned long offset, unsigned count);

/*
* Deallocate memory of _node_ and its children
*
* Returns null pointer
*/
struct vnode* vnode_remove(struct vnode *node);

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
* Wrap _buf_ of size _n_ with a Twrite and a Tread message to _vnode_
*/
enum vfs_error vnode_wrap(const struct vnode *vnode, unsigned char *buf, size_t n);

#endif /* _MOD_VFS_VNODE_H */
