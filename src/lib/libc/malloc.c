/*
* Memory allocation functions using the buddy system
*/

#include "stdlib.h"

#include <stddef.h>

#include "internal/malloc.h"

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

void free(void *ptr) {
    if (!ptr) return;

    block_t *block = (block_t *) ((size_t) ptr - sizeof(block_t));
    block->available = 1;

    _coalesce_blocks(block);
}
