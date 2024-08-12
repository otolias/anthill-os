#include "stdlib.h"

#include <sys/types.h>

#include "internal/stdlib.h"

void free(void *ptr) {
    if (!ptr) return;

    struct block *block = (struct block *) ((size_t) ptr - sizeof(struct block));
    block->available = 1;

    block_coalesce(block);
}
