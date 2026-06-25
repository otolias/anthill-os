#include "string.h"

#include <stddef.h>
#include <stdlib.h>

char *strdup(const char *s) {
    size_t size = strlen(s) + 1;
    char *r = malloc(size);
    if (!r) return NULL;

    memcpy(r, s, size);
    return r;
}
