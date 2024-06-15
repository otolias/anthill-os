#include "fcall.h"

#include <pstring.h>
#include <string.h>
#include <sys/types.h>

static unsigned _put_uchar(unsigned char *ptr, unsigned short val) {
    ptr[0] = val;
    return CHAR_SIZE;
}

static unsigned _put_ushort(unsigned char *ptr, unsigned short val) {
    ptr[0] = val; ptr[1] = val >> 8;
    return SHRT_SIZE;
}

static unsigned _put_uint(unsigned char *ptr, unsigned int val) {
    ptr[0] = val; ptr[1] = val >> 8; ptr[2] = val >> 16; ptr[3] = val >> 24;
    return INT_SIZE;
}

static unsigned _put_ulong(unsigned char *ptr, unsigned long val) {
    ptr[0] = val; ptr[1] = val >> 8; ptr[2] = val >> 16; ptr[3] = val >> 24;
    ptr[4] = val >> 32; ptr[5] = val >> 40; ptr[6] = val >> 48; ptr[7] = val >> 56;
    return LONG_SIZE;
}

static unsigned _put_pstring(unsigned char *ptr, const pstring *pstr) {
    unsigned i = _put_ushort(ptr, pstr->len);
    const unsigned char *c = pstr->s;

    for (; i < pstrlen(pstr); i++)
        ptr[i] = *c++;

    return i;
}

static unsigned _put_qid(unsigned char *ptr, const struct qid *qid) {
    unsigned n = 0;
    n += _put_uint(ptr + n, qid->type);
    n += _put_uint(ptr + n , qid->version);
    n += _put_ulong(ptr + n, qid->id);
    return n;
}

unsigned fcall_msg_to_buf(const fcall *fcall, unsigned char *buf, unsigned length) {
    unsigned char *ptr = buf;

    unsigned size = fcall_msg_size(fcall);

    if (size == 0 || size > length)
        return 0;

    ptr += _put_uint(ptr, fcall_msg_size(fcall));
    ptr += _put_uchar(ptr, fcall->type);
    ptr += _put_ushort(ptr, fcall->tag);

    switch (fcall->type) {
        case Tversion:
        case Rversion:
            ptr += _put_uint(ptr, fcall->msize);
            ptr += _put_pstring(ptr, fcall->version);
            break;

        case Rerror:
            ptr += _put_pstring(ptr, fcall->ename);
            break;

        case Tattach:
            ptr += _put_uint(ptr, fcall->fid);
            ptr += _put_uint(ptr, fcall->afid);
            ptr += _put_pstring(ptr, fcall->uname);
            ptr += _put_pstring(ptr, fcall->aname);
            break;

        case Rattach:
            ptr += _put_qid(ptr, &fcall->qid);
            break;

        case Twalk: {
            ptr += _put_uint(ptr, fcall->fid);
            ptr += _put_uint(ptr, fcall->newfid);
            ptr += _put_ushort(ptr, fcall->nwname);

            char *s = (char *) fcall->wname;
            for (size_t i = 0; i < fcall->nwname; i++) {
                pstring *pstr = (pstring *) s;
                ptr += _put_pstring(ptr, pstr);
                s += pstrlen(pstr);
            }

            break;
        }

        case Rwalk:
            ptr += _put_ushort(ptr, fcall->nwqid);

            for (size_t i = 0; i < fcall->nwqid; i++)
                ptr += _put_qid(ptr, &fcall->wqid[i]);

            break;

        case Topen:
            ptr += _put_uint(ptr, fcall->fid);
            ptr += _put_uchar(ptr, fcall->mode);
            break;

        case Ropen:
        case Rcreate:
            ptr += _put_qid(ptr, &fcall->qid);
            ptr += _put_uint(ptr, fcall->iounit);
            break;

        case Tcreate:
            ptr += _put_uint(ptr, fcall->fid);
            ptr += _put_pstring(ptr, fcall->name);
            ptr += _put_uint(ptr, fcall->perm);
            ptr += _put_uchar(ptr, fcall->mode);
            break;

        case Tread:
            ptr += _put_uint(ptr, fcall->fid);
            ptr += _put_ulong(ptr, fcall->offset);
            ptr += _put_uint(ptr, fcall->count);
            break;

        case Rread:
            ptr += _put_uint(ptr, fcall->count);
            memmove(ptr, fcall->data, fcall->count);
            ptr += fcall->count;
            break;

        case Twrite:
            ptr += _put_uint(ptr, fcall->fid);
            ptr += _put_ulong(ptr, fcall->offset);
            ptr += _put_uint(ptr, fcall->count);
            memmove(ptr, fcall->data, fcall->count);
            ptr += fcall->count;
            break;

        case Rwrite:
            ptr += _put_uint(ptr, fcall->count);
            break;

        case Tclunk:
        case Tstat:
            ptr += _put_uint(ptr, fcall->fid);
            break;

        case Rstat:
            ptr += _put_ushort(ptr, fcall->nstat);
            memmove(ptr, fcall->stat, fcall->nstat);
            ptr += fcall->nstat;
            break;

        case Rclunk:
        default:
            break;
    };

    return ptr - buf;
}
