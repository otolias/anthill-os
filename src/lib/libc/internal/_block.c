#include "internal/stdlib.h"

#include <stddef.h>
#include <sys/mman.h>

struct block *first_block;

/*
* Get more memory from the allocator for _block_
*/
static short _expand(struct block *block) {
    struct block *new_block = mmap(0, MALLOC_BLOCK_SIZE, 0, MAP_ANONYMOUS, 0, 0);

    if(!new_block)
        return 1;

    new_block->available = 1;
    new_block->k = MALLOC_MAX_ORDER;
    new_block->next = NULL;

    if (block) {
        new_block->prev = block;
        block->next = new_block;
    } else {
        first_block = new_block;
    }

    return 0;
}

/*
 * Release memory from the allocator from _block_
*/
static void _shrink(struct block *block) {
    if (block->next)
        return;

    struct block *prev = block->prev;
    prev->next = NULL;

    munmap(block, MALLOC_BLOCK_SIZE);
}

/*
* Split the given _block_ to two blocks of lower k
*/
static void _split(struct block *block) {
    block->k--;
    struct block *new_block = (struct block *) ((size_t) block + (1 << block->k));
    new_block->available = 1;
    new_block->k = block->k;
    new_block->next = block->next;
    new_block->prev = block;
    block->next = new_block;
}

void block_coalesce(struct block *block) {
    if (block->k == MALLOC_MAX_ORDER) {
        _shrink(block);
        return;
    }

    struct block *buddy = (struct block *) ((size_t) block ^ (size_t) (1 << block->k));

    while (buddy && buddy->available) {
        if (buddy < block) {
            struct block *temp = buddy;
            buddy = block;
            block = temp;
        }

        block->k++;
        block->next = buddy->next;

        buddy = (struct block *) ((size_t) block ^ (size_t) (1 << block->k));
    }
}

void block_deinit(void) {
    struct block *current_block = first_block;
    while (current_block) {
        struct block *next_block = current_block->next;

        while (next_block) {
            if ((size_t) next_block % (1 << MALLOC_MAX_ORDER) == 0)
                break;

            next_block = next_block->next;
        }

        munmap(current_block, MALLOC_BLOCK_SIZE);
        current_block = next_block;
    }
}

struct block* block_find(char order) {
    struct block *current_block = first_block;
    struct block *last_block = NULL;

    while (current_block) {
        if (!current_block->available || current_block->k < order) {
            last_block = current_block;
            current_block = current_block->next;
            continue;
        }

        if (current_block->k > order) {
            _split(current_block);
            continue;
        }

        if (current_block->k == order) {
            current_block->available = 0;
            return current_block;
        }
    }

    if (_expand(last_block) == 0)
        return block_find(order);

    return NULL;
}

size_t block_get_size(void *ptr) {
    const struct block *block = (struct block *) ((size_t) ptr - sizeof(struct block));
    const struct block *current_block = first_block;

    while (current_block) {
        if (current_block == block)
            break;

        current_block = current_block->next;
    }

    if (!current_block) return 0;

    return 1 << current_block->k;
}
