#include "fcall.h"

#include <stddef.h>
#include <string.h>
#include <sys/types.h>

unsigned fcall_msg_size(const fcall *fcall) {
    size_t size = 0;

    size += INT_SIZE; /* size */
    size += CHAR_SIZE; /* type */
    size += INT_SIZE; /* fid */
    size += SHRT_SIZE; /* tag */

    switch (fcall->type) {
        case Tversion:
        case Rversion:
            size += INT_SIZE; /* msize */
            size += strlen(fcall->version) + 1; /* version */
            size += strlen(fcall->channel) + 1; /* channel */
            break;
        default:
            return 0;
    }

    return size;
}
