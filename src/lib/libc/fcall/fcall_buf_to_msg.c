#include "fcall.h"

#include <pstring.h>

static unsigned char _get_uchar(const unsigned char *ptr) {
    return *ptr;
}

static unsigned short _get_ushort(const unsigned char *ptr) {
    return (unsigned char) ptr[0] | ptr[1] << 8;
}

static unsigned int _get_uint(const unsigned char *ptr) {
    return (unsigned char) ptr[0] | (unsigned char) ptr[1] << 8 |
        (unsigned char) ptr[2] << 16 | ptr[3] << 24;
}

static unsigned long _get_ulong(const unsigned char *ptr) {
    return _get_uint(ptr) | (unsigned long) _get_uint(ptr + 4) << 32;
}

unsigned fcall_buf_to_msg(unsigned char *buf, fcall *fcall) {
    unsigned char *ptr = buf;

    unsigned size = _get_uint(ptr);
    ptr += INT_SIZE;

    fcall->type = _get_uchar(ptr);
    ptr += CHAR_SIZE;
    fcall->tag = _get_ushort(ptr);
    ptr += SHRT_SIZE;

    switch (fcall->type) {
        case Tversion:
        case Rversion:
            fcall->msize = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->version = (pstring *) ptr;
            ptr += pstrlen(fcall->version);
            break;

        case Rerror:
            fcall->ename = (pstring *) ptr;
            ptr += pstrlen(fcall->ename);
            break;

        case Tattach:
            fcall->fid = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->afid = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->uname = (pstring *) ptr;
            ptr += pstrlen(fcall->uname);
            fcall->aname = (pstring *) ptr;
            ptr += pstrlen(fcall->aname);
            break;

        case Rattach:
            fcall->qid.type = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->qid.version = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->qid.id = _get_ulong(ptr);
            ptr += LONG_SIZE;
            break;

        case Twalk:
            fcall->fid = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->newfid = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->nwname = _get_ushort(ptr);
            ptr += SHRT_SIZE;
            fcall->wname = (pstring *) ptr;
            for (unsigned short i = 0; i < fcall->nwname; i++)
                ptr += pstrlen((pstring *) ptr);
            break;

        case Rwalk:
            fcall->nwqid = _get_ushort(ptr);
            ptr += SHRT_SIZE;
            fcall->wqid = (struct qid *) ptr;
            ptr += fcall->nwqid * sizeof(struct qid);
            break;

        case Topen:
            fcall->fid = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->mode = _get_uchar(ptr);
            ptr += CHAR_SIZE;
            break;

        case Ropen:
        case Rcreate:
            fcall->qid.type = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->qid.version = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->qid.id = _get_ulong(ptr);
            ptr += LONG_SIZE;
            fcall->iounit = _get_uint(ptr);
            ptr += INT_SIZE;
            break;

        case Tcreate:
            fcall->fid = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->name = (pstring *) ptr;
            ptr += pstrlen(fcall->name);
            fcall->perm = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->mode = _get_uchar(ptr);
            ptr += CHAR_SIZE;
            break;

        case Tread:
            fcall->fid = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->offset = _get_ulong(ptr);
            ptr += LONG_SIZE;
            fcall->count = _get_uint(ptr);
            ptr += INT_SIZE;
            break;

        case Rread:
            fcall->count = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->data = ptr;
            ptr += fcall->count;
            break;

        case Twrite:
            fcall->fid = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->offset = _get_ulong(ptr);
            ptr += LONG_SIZE;
            fcall->count = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->data = ptr;
            ptr += fcall->count;
            break;

        case Rwrite:
            fcall->count = _get_uint(ptr);
            ptr += INT_SIZE;
            break;

        case Tclunk:
        case Tstat:
            fcall->fid = _get_uint(ptr);
            ptr += INT_SIZE;
            break;

        case Rclunk:
            break;

        case Rstat:
            fcall->nstat = _get_ushort(ptr);
            ptr += SHRT_SIZE;
            fcall->stat.qid.type = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->stat.qid.version = _get_uint(ptr);
            ptr += INT_SIZE;
            fcall->stat.qid.id = _get_ulong(ptr);
            ptr += LONG_SIZE;
            fcall->stat.length = _get_ulong(ptr);
            ptr += LONG_SIZE;
            fcall->stat.name = (pstring *) ptr;
            ptr += pstrlen(fcall->stat.name);
            break;

        default:
            return 0;
    }

    if (size != ptr - buf)
        return 0;

    return size;
}
