#include "pstring.h"

#include <stddef.h>
#include <stdlib.h>

char* pstrtoz(char *dest, const pstring *src, size_t n) {
    char *str;

    if (dest && n < src->len) return NULL;

    if (dest)
        str = dest;
    else {
        str = malloc(src->len + 1);
        if (!str) return NULL;
    }

    for (unsigned short i = 0; i < src->len; i++)
        str[i] = src->s[i];

    str[src->len] = 0;

    return str;
}
