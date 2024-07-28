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

extern mqd_t vfs_in;
extern unsigned long id_count;
extern unsigned tag_count;
extern unsigned fid_count;

/*
* Setup root node
*/
enum vfs_error vfs_init(void);

#endif /* _MOD_VFS_H */
