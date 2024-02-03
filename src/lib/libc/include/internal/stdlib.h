/*
* Memory allocation init and helper functions
*
* Memory allocation uses the buddy system, meaning that the available
* memory pool starts with a single block of size 2 to the power of 12,
* and is then split or coalesced as needed.
*/

#ifndef _INTERNAL_STDLIB_H
#define _INTERNAL_STDLIB_H

typedef struct {
    char available;
    char k;
    void *prev;
    void *next;
} block_t;

/*
* Coalesce blocks
*/
void _coalesce_blocks(block_t *block);

/*
* Iterate through the available blocks and find one with k equal to _order_
*/
block_t* _find_available_block(char order);

/*
* Initialise first and last blocks
*/
void _malloc_init(void *malloc_start);

/*
* Split the given _block_ to two blocks of lower k
*/
void _split_block(block_t *block);

#endif /* _INTERNAL_STDLIB_H */
