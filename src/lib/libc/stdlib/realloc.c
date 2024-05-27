#include "stdlib.h"

#include <errno.h>
#include <stddef.h>
#include <string.h>

#include "internal/stdlib.h"

void* realloc(void *ptr, size_t size) {
    if (size == 0)
        { errno = EINVAL; return NULL; }

    size_t n = 0;

    if (ptr) {
        n = block_get_size(ptr);
        if (n == 0)
            { errno = EINVAL; return NULL; }
    }

    void *result = malloc(size);
    if (!result)
        { errno = ENOMEM; return NULL; }

    memcpy(result, ptr, n < size ? n : size);
    free(ptr);

    return result;
}
