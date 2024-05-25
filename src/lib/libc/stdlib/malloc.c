#include "stdlib.h"

#include <stddef.h>
#include <errno.h>
#include <sys/types.h>

#include "internal/stdlib.h"

void* malloc(size_t size) {
    if (size == 0)
        { errno = EINVAL; return NULL; }

    size_t actual_size = size + sizeof(block_t);

    if (actual_size > MALLOC_BLOCK_SIZE)
        { errno = ENOMEM; return NULL; }

    int j;
    for (j = 0; actual_size; j++) {
        actual_size = actual_size >> 1;
    }

    block_t *available_block = block_find(j);

    if(!available_block) {
        errno = ENOMEM;
        return NULL;
    }

    return (void *) ((size_t) available_block + sizeof(block_t));
}
