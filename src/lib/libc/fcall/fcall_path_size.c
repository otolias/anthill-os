#include "fcall.h"

#include <limits.h>
#include <pstring.h>

unsigned short fcall_path_size(fcall *fcall) {
    size_t total_length = 0;
    char *ptr = (char *) fcall->wname;

    for (unsigned short i = 0; i < fcall->nwname; i++) {
        const pstring *pstr = (pstring *) ptr;

        total_length += pstr->len;
        if (fcall->nwname - i != 1)
            total_length++; /* Path delimiter (/) */

        ptr += pstrlen(pstr);
    }

    if (total_length > SHRT_MAX)
        return 0;

    return total_length;
}
