#include "string.h"

#include <stddef.h>
#include <stdlib.h>

char *strdup(const char *s) {
    size_t size = strlen(s);
    char *r = malloc(size + 1);
    if (!r) return NULL;

    memcpy(r, s, size);
    return r;
}
