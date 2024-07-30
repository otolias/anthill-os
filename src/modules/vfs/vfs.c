#include "vfs.h"

#include <errno.h>
#include <mqueue.h>
#include <pstring.h>
#include <stddef.h>

#include "vnode.h"

mqd_t vfs_in = -1;
unsigned id_count = 0;;
unsigned tag_count = 0;

static pstring *uname;
static pstring *aname;

/*
* Setup boot directory under _root_
*/
static enum vfs_error _setup_boot(struct vnode *root) {
    struct vnode *boot = vnode_add_child(root);
    if (!boot) return VFS_NOMEM;

    boot->name = pstrconv(NULL, "boot", 0);
    if (!boot->name)
        { vnode_remove(boot); return VFS_NOMEM; }

    errno = 0;
    boot->mq_id = mq_open("vfs/mod/rd", O_WRONLY);
    if (boot->mq_id == -1)
        { vnode_remove(boot); return VFS_NOTFOUND; }

    char buf[VFS_MAX_MSG_LEN];
    struct vfs_msg vfs_msg;
    unsigned fid = id_count++;

    vfs_msg.fcall.type = Tattach;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = fid;
    vfs_msg.fcall.afid = NOFID;
    vfs_msg.fcall.uname = uname;
    vfs_msg.fcall.aname = aname;
    vfs_msg.mq_id = vfs_in;

    if (!vfs_msg_send(&vfs_msg, buf, boot->mq_id, vfs_in))
        { vnode_remove(boot); return VFS_MQERR; }

    if (vfs_msg.fcall.type != Rattach)
        { vnode_remove(boot); return VFS_MQERR; }

    boot->qid = vfs_msg.fcall.qid;
    boot->children = NULL;
    boot->children_no = 0;
    boot->mount_node = NULL;

    return VFS_OK;
}

/*
* Setup pl011 driver under _dev_
*/
static enum vfs_error _setup_pl011(struct vnode *dev) {
    struct vnode *pl011 = vnode_add_child(dev);
    if (!pl011) return VFS_NOTFOUND;

    pl011->name = pstrconv(NULL, "pl011", 0);
    if (!pl011->name)
        { vnode_remove(pl011); return VFS_NOMEM; }

    errno = 0;
    pl011->mq_id = mq_open("vfs/mod/pl011", O_WRONLY);
    if (pl011->mq_id == -1)
        { vnode_remove(pl011); return VFS_NOTFOUND; }

    char buf[VFS_MAX_MSG_LEN];
    struct vfs_msg vfs_msg;
    unsigned fid = id_count++;

    vfs_msg.fcall.type = Tattach;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = fid;
    vfs_msg.fcall.afid = NOFID;
    vfs_msg.fcall.uname = uname;
    vfs_msg.fcall.aname = aname;
    vfs_msg.mq_id = vfs_in;

    if (!vfs_msg_send(&vfs_msg, buf, pl011->mq_id, vfs_in))
        { vnode_remove(pl011); return VFS_MQERR; }

    if (vfs_msg.fcall.type != Rattach)
        { vnode_remove(pl011); return VFS_MQERR; }

    pl011->qid = vfs_msg.fcall.qid;
    pl011->children = NULL;
    pl011->children_no = 0;
    pl011->mount_node = NULL;

    return VFS_OK;
}

/*
* Setup uart driver under _dev_
*/
static enum vfs_error _setup_uart(struct vnode *dev) {
    struct vnode *uart = vnode_add_child(dev);
    if (!uart) return VFS_NOMEM;

    uart->name = pstrconv(NULL, "uart", 0);
    if (!uart->name)
        { vnode_remove(uart); return VFS_NOMEM; }

    errno = 0;
    uart->mq_id = mq_open("vfs/mod/uart", O_WRONLY);
    if (uart->mq_id == -1)
        { vnode_remove(uart); return VFS_NOTFOUND; }

    char buf[VFS_MAX_MSG_LEN];
    struct vfs_msg vfs_msg;
    unsigned fid = id_count++;

    vfs_msg.fcall.type = Tattach;
    vfs_msg.fcall.tag = tag_count++;
    vfs_msg.fcall.fid = fid;
    vfs_msg.fcall.afid = NOFID;
    vfs_msg.fcall.uname = uname;
    vfs_msg.fcall.aname = aname;
    vfs_msg.mq_id = vfs_in;

    if (!vfs_msg_send(&vfs_msg, buf, uart->mq_id, vfs_in))
        { vnode_remove(uart); return VFS_MQERR; }

    if (vfs_msg.fcall.type != Rattach)
        { vnode_remove(uart); return VFS_MQERR; }

    uart->qid = vfs_msg.fcall.qid;
    uart->children = NULL;
    uart->children_no = 0;
    uart->mount_node = NULL;

    return VFS_OK;
}

/*
* Setup dev directory under _root_
*/
static enum vfs_error _setup_dev(struct vnode *root) {
    struct vnode *dev = vnode_add_child(root);
    if (!dev) return VFS_NOMEM;

    dev->name = pstrconv(NULL, "dev", 0);
    if (!dev->name)
        { vnode_remove(dev); return VFS_NOMEM; }

    dev->qid.type = QTDIR;
    dev->qid.version = 0;
    dev->qid.id = id_count++;
    dev->children = NULL;
    dev->children_no = 0;
    dev->mount_node = NULL;

    enum vfs_error err;

    err = _setup_pl011(dev);
    if (err != VFS_OK)
        { vnode_remove(dev); return err; }

    err = _setup_uart(dev);
    if (err != VFS_OK)
        { vnode_remove(dev); return err; }

    return VFS_OK;
}

enum vfs_error vfs_init() {
    errno = 0;
    vfs_in = mq_open("vfs/client", O_RDONLY | O_CREAT | O_EXCL);
    if (vfs_in == -1) return VFS_NOMEM;

    struct vnode *root = vnode_get_root();;

    root->qid.type = QTDIR;
    root->qid.version = 0;
    root->qid.id = id_count++;
    root->children = NULL;
    root->children_no = 0;
    root->mount_node = NULL;
    root->mq_id = -1;

    char uname_buf[16];
    char aname_buf[16];
    uname = pstrconv(uname_buf, "vfs/client", 16);
    aname = pstrconv(aname_buf, "/", 16);

    enum vfs_error err;

    err = _setup_boot(root);
    if (err != VFS_OK)
        { vnode_remove(root); return err; }

    err = _setup_dev(root);
    if (err != VFS_OK)
        { vnode_remove(root); return err; }

    return VFS_OK;
}
