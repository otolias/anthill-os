#include "fcall.h"

#include <stddef.h>
#include <string.h>
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
            size += SHRT_SIZE + strlen(fcall->version); /* version */
            break;

        case Rerror:
            size += SHRT_SIZE + strlen(fcall->ename); /* ename */
            break;

        case Tattach:
            size += INT_SIZE; /* fid */
            size += INT_SIZE; /* afid */
            size += SHRT_SIZE + strlen(fcall->uname); /* uname */
            size += SHRT_SIZE + strlen(fcall->aname); /* aname */
            break;

        case Rattach:
            size += sizeof(qid);
            break;

        case Topen:
            size += INT_SIZE; /* fid */
            size += CHAR_SIZE; /* mode */
            break;

        case Ropen:
        case Rcreate:
            size += sizeof(qid); /* qid */
            size += INT_SIZE; /* iounit */
            break;

        case Tcreate:
            size += INT_SIZE; /* fid */
            size += SHRT_SIZE + strlen(fcall->name); /* name */
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

        default:
            return 0;
    }

    return size;
}
