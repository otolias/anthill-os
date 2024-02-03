#include "stdlib.h"

#include <sys/types.h>

#include "internal/stdlib.h"

void* malloc(size_t size) {
    size_t actual_size = size + sizeof(block_t);

    int j;
    for (j = 0; actual_size; j++) {
        actual_size = actual_size >> 1;
    }

    block_t *available_block = _find_available_block(j);

    /* TODO: If it fails, get more memory */

    return (void *) ((size_t) available_block + sizeof(block_t));
}
