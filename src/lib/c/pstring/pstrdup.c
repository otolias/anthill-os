#include "pstring.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

pstring* pstrdup(void *dest, const pstring* src, size_t n) {
    size_t size = pstrlen(src);
    pstring *pstr;

    if (dest) {
        if (size > n) return NULL;
        pstr = dest;
    } else {
        pstr = malloc(size);
        if (!pstr) return NULL;
    }

    pstr->len = src->len;
    for (unsigned short i = 0; i < pstr->len; i++)
        pstr->s[i] = src->s[i];

    return pstr;
}
