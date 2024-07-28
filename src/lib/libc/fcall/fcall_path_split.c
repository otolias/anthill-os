#include "fcall.h"

#include <limits.h>
#include <pstring.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

int fcall_path_split(pstring **wname, const char *path) {
    if (strcmp(path, "/") == 0) {
        *wname = NULL;
        return 0;
    }

    size_t nwname = strcnt(path, '/');
    if (nwname > SHRT_MAX)
        return -1;

    /* Allocate space */
    *wname = malloc(strlen(path) + nwname * sizeof(pstring));
    if (!*wname) return -1;

    char *ptr = (char *) *wname;
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

    return nwname;
}
