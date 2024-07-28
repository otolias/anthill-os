#include "vfs.h"

#include <errno.h>
#include <fcall.h>
#include <pstring.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/vfs.h>

#include "vnode.h"

mqd_t vfs_in = -1;
unsigned long id_count = 0;
unsigned tag_count = 0;
unsigned fid_count = 0;

enum vfs_error vfs_init(void) {
    errno = 0;
    vfs_in = mq_open("vfs/client", O_RDONLY | O_CREAT | O_EXCL);
    if (vfs_in == -1) return VFS_NOMEM;

    struct vnode *root = vnode_get_root();

    errno = 0;
    root->mq_id = mq_open("vfs/mod/rd", O_WRONLY);
    if (root->mq_id == -1)
        { vnode_remove(root); return VFS_NOTFOUND; }

    char buf[VFS_MAX_MSG_LEN];
    char uname_buf[16];
    char aname_buf[16];
    pstring *uname = pstrconv(uname_buf, "vfs/client", 16);
    pstring *aname = pstrconv(aname_buf, "/", 16);
    unsigned fid;
    struct vfs_msg vfs_msg;

    /* Setup root */
    fid = fid_count++;
    vfs_msg.fcall.type = Tattach;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = fid;
    vfs_msg.fcall.afid = NOFID;
    vfs_msg.fcall.uname = uname;
    vfs_msg.fcall.aname = aname;
    vfs_msg.mq_id = vfs_in;

    if (!vfs_msg_send(&vfs_msg, buf, root->mq_id, vfs_in))
        { vnode_remove(root); return VFS_NOTFOUND; }

    if (vfs_msg.fcall.type != Rattach)
        { vnode_remove(root); return VFS_NOTFOUND; }

    root->qid.type = vfs_msg.fcall.qid.type;
    root->qid.id = id_count++;
    root->qid.version = 0;
    root->fid = fid;
    root->children = NULL;
    root->children_no = 0;

    /* Setup /dev */
    struct vnode *dev = vnode_add_child(root);
    if (!dev) return VFS_NOTFOUND;

    dev->name = pstrconv(NULL, "dev", 0);
    dev->qid.type = QTDIR;
    dev->qid.id = id_count++;
    dev->qid.version = 0;
    dev->mount_node = root;
    dev->mq_id = root->mq_id;

    /* Setup /dev/uart */
    struct vnode *uart = vnode_add_child(dev);
    if (!uart) return VFS_NOTFOUND;

    errno = 0;
    uart->mq_id = mq_open("vfs/mod/uart", O_WRONLY);
    if (uart->mq_id == -1)
        { vnode_remove(root); return VFS_NOTFOUND; }

    fid = fid_count++;
    vfs_msg.fcall.type = Tattach;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = fid;
    vfs_msg.fcall.afid = NOFID;
    vfs_msg.fcall.uname = uname;
    vfs_msg.fcall.aname = aname;
    vfs_msg.mq_id = vfs_in;

    if (!vfs_msg_send(&vfs_msg, buf, uart->mq_id, vfs_in))
        { vnode_remove(root); return VFS_NOTFOUND; }

    if (vfs_msg.fcall.type != Rattach)
        { vnode_remove(root); return VFS_NOTFOUND; }

    uart->name = pstrconv(NULL, "uart", 0);
    uart->qid.type = vfs_msg.fcall.qid.type;
    uart->qid.id = id_count++;
    uart->qid.version = 0;
    uart->fid = fid;

    /* Setup /dev/pl011 */
    struct vnode *pl011 = vnode_add_child(dev);
    if (!pl011) return VFS_NOTFOUND;

    errno = 0;
    pl011->mq_id = mq_open("vfs/mod/pl011", O_WRONLY);
    if (pl011->mq_id == -1)
        { vnode_remove(root); return VFS_NOTFOUND; }

    fid = fid_count++;
    vfs_msg.fcall.type = Tattach;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = fid;
    vfs_msg.fcall.afid = NOFID;
    vfs_msg.fcall.uname = uname;
    vfs_msg.fcall.aname = aname;
    vfs_msg.mq_id = vfs_in;

    if (!vfs_msg_send(&vfs_msg, buf, pl011->mq_id, vfs_in))
        { vnode_remove(root); return VFS_NOTFOUND; }

    if (vfs_msg.fcall.type != Rattach)
        { vnode_remove(root); return VFS_NOTFOUND; }

    pl011->name = pstrconv(NULL, "pl011", 0);
    pl011->qid.type = vfs_msg.fcall.qid.type;
    pl011->qid.id = id_count++;
    pl011->qid.version = 0;
    pl011->fid = fid;

    return VFS_OK;
}
