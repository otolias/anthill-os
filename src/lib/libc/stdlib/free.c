#include "stdlib.h"

#include <sys/types.h>

#include "internal/stdlib.h"

void free(void *ptr) {
    if (!ptr) return;

    block_t *block = (block_t *) ((size_t) ptr - sizeof(block_t));
    block->available = 1;

    _coalesce_blocks(block);
}
