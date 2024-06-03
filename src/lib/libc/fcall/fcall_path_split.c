#include "fcall.h"

#include <limits.h>
#include <pstring.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

unsigned short fcall_path_split(fcall *fcall, const char *path) {
    size_t nwname = strcnt(path, '/');
    if (nwname > SHRT_MAX)
        return 0;

    /* Allocate space */
    pstring *wname = malloc(strlen(path) + nwname * sizeof(pstring));
    if (!wname) return 0;

    char *ptr = (char *) wname;
    const char *l, *r;

    r = strchr(path, '/');

    do {
        l = r; r = strchr(l + 1, '/');
        size_t len = (r ? r - l : strlen(l)) - 1;

        pstring *element = (pstring *) ptr;
        element->len = len;
        memcpy(&element->s, l + 1, len);

        ptr += len + sizeof(pstring);
    } while(r);

    fcall->nwname = nwname;
    fcall->wname = wname;

    return nwname;
}
