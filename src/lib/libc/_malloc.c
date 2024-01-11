/*
* Memory allocation init and helper functions
*
* Memory allocation uses the buddy system, meaning that the available
* memory pool starts with a single block of size 2 to the power of 12,
* and is then split or coalesced as needed.
*/

#include "internal/malloc.h"

#include <stddef.h>

block_t *first_block;

void _split_block(block_t *block) {
    block->k--;
    block_t *new_block = (block_t *) ((size_t) block + (2 << block->k));
    new_block->available = 1;
    new_block->k = block->k;
    new_block->next = block->next;
    new_block->prev = block;
    block->next = new_block;
}

block_t* _find_available_block(char order) {
    block_t *current_block = first_block;

    while (current_block) {
        if (!current_block->available || current_block->k < order) {
            current_block = current_block->next;
            continue;
        }

        if (current_block->k > order) {
            _split_block(current_block);
            continue;
        }

        if (current_block->k == order) {
            current_block->available = 0;
            return current_block;
        }
    }

    return NULL;
}

void _coalesce_blocks(block_t *block) {
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

void _malloc_init(void *malloc_start) {
    block_t *block = (block_t *) malloc_start;

    block->available = 1;
    block->k = 12;
    block->prev = NULL;
    block->next = NULL;

    first_block = block;
}
