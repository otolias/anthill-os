#include "internal/stdlib.h"

#include <stddef.h>
#include <sys/mman.h>

static short _expand(block_t *block);
static void _shrink(block_t *block);
static void _split(block_t *block);

block_t *first_block;

/*
* Get more memory for the allocator from _block_
*/
static short _expand(block_t *block) {
    block_t *new_block = mmap(0, MALLOC_BLOCK_SIZE, 0, MAP_ANONYMOUS, 0, 0);

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
static void _shrink(block_t *block) {
    if (block->next)
        return;

    block_t *prev = block->prev;
    prev->next = NULL;

    munmap(block, MALLOC_BLOCK_SIZE);
}

/*
* Split the given _block_ to two blocks of lower k
*/
static void _split(block_t *block) {
    block->k--;
    block_t *new_block = (block_t *) ((size_t) block + (2 << block->k));
    new_block->available = 1;
    new_block->k = block->k;
    new_block->next = block->next;
    new_block->prev = block;
    block->next = new_block;
}

void block_coalesce(block_t *block) {
    if (block->k == MALLOC_MAX_ORDER) {
        _shrink(block);
        return;
    }

    block_t *buddy = (block_t *) ((size_t) block ^ (size_t) (1 << block->k));

    while (buddy && buddy->available) {
        if (buddy > block) {
            block_t *temp = buddy;
            buddy = block;
            block = temp;
        }

        block->k++;
        block->next = buddy->next;

        buddy = (block_t *) ((size_t) block ^ (size_t) (1 << block->k));
    }
}

block_t* block_find(char order) {
    block_t *current_block = first_block;

    while (current_block) {
        if (!current_block->available || current_block->k < order) {
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

    if (_expand(current_block) == 0)
        return block_find(order);

    return NULL;
}
