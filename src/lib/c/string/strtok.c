#include "string.h"

#include <stddef.h>

char *strtok(char *restrict s, const char *restrict sep) {
    static char *prev;

    if (!s) s = prev;
    if (!s) return NULL;

    prev = s + strcspn(s, sep);
    if (!*prev)
        prev = NULL;
    else {
        *(prev++) = 0;
        prev += strspn(prev, sep);
    }

    return s;
}
