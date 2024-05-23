#include "pstring.h"

#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

pstring* pstrconv(void *dest, const char *src, size_t n) {
    size_t len = strlen(src);
    size_t size = len + sizeof(unsigned short);
    pstring *pstr;

    if (len > SHRT_MAX) return NULL;
    if (dest && size > n) return NULL;

    if (dest)
        pstr = dest;
    else {
        pstr = malloc(size);
        if (!pstr) return NULL;
    }

    pstr->len = len;
    for (unsigned short i = 0; i < len; i++)
        pstr->s[i] = src[i];

    return pstr;
}
