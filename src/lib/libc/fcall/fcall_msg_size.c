#include "fcall.h"

#include <pstring.h>
#include <stddef.h>
#include <sys/types.h>

unsigned fcall_msg_size(const fcall *fcall) {
    size_t size = 0;

    size += INT_SIZE; /* size */
    size += CHAR_SIZE; /* type */
    size += SHRT_SIZE; /* tag */

    switch (fcall->type) {
        case Tversion:
        case Rversion:
            size += INT_SIZE; /* msize */
            size += pstrlen(fcall->version); /* version */
            break;

        case Rerror:
            size += pstrlen(fcall->ename); /* ename */
            break;

        case Tattach:
            size += INT_SIZE; /* fid */
            size += INT_SIZE; /* afid */
            size += pstrlen(fcall->uname); /* uname */
            size += pstrlen(fcall->aname); /* aname */
            break;

        case Rattach:
            size += sizeof(struct qid);
            break;

        case Twalk: {
            size += INT_SIZE; /* fid */
            size += INT_SIZE; /* newfid */
            size += SHRT_SIZE; /* nwname */

            unsigned char *ptr = (unsigned char *) fcall->wname;
            for (unsigned short i = 0; i < fcall->nwname; i++) {
                size_t len = pstrlen((pstring *) ptr);
                size += len;
                ptr += len;
            } /* nwname * wname */
            break;
        }

        case Rwalk:
            size += SHRT_SIZE; /* nwqid */
            size += sizeof(struct qid) * fcall->nwqid; /* wqid */
            break;

        case Topen:
            size += INT_SIZE; /* fid */
            size += CHAR_SIZE; /* mode */
            break;

        case Ropen:
        case Rcreate:
            size += sizeof(struct qid); /* qid */
            size += INT_SIZE; /* iounit */
            break;

        case Tcreate:
            size += INT_SIZE; /* fid */
            size += pstrlen(fcall->name); /* name */
            size += INT_SIZE; /* perm */
            size += CHAR_SIZE; /* mode */
            break;

        case Tread:
            size += INT_SIZE; /* fid */
            size += LONG_SIZE; /* offset */
            size += INT_SIZE; /* count */
            break;

        case Rread:
            size += INT_SIZE; /* count */
            size += fcall->count; /* data */
            break;

        case Twrite:
            size += INT_SIZE; /* fid */
            size += LONG_SIZE; /* offset */
            size += INT_SIZE; /* count */
            size += fcall->count; /* data */
            break;

        case Rwrite:
            size += INT_SIZE; /* count */
            break;

        case Tclunk:
        case Tstat:
            size += INT_SIZE; /* fid */
            break;

        case Rclunk:
            break;

        case Rstat:
            size += SHRT_SIZE; /* nstat size */
            size += fcall->nstat; /* stat */
            break;

        case Tmount:
            size += INT_SIZE; /* fid */
            size += INT_SIZE; /* mfid */
            break;

        case Rmount:
            size += sizeof(struct qid);
            break;

        default:
            return 0;
    }

    return size;
}
