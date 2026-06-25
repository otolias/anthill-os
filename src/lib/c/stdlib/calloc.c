#include "stdlib.h"

#include <errno.h>
#include <stddef.h>
#include <string.h>

void *calloc(size_t nelem, size_t elsize) {
    if (nelem == 0 || elsize == 0) {
        errno = EINVAL;
        return NULL;
    }

    size_t size = nelem * elsize;
    void *p = malloc(size);
    if (!p)
        return NULL;

    memset(p, 0, size);
    return p;
}
