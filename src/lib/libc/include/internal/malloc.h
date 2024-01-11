#ifndef _INTERNAL_MALLOC_H
#define _INTERNAL_MALLOC_H

typedef struct {
    char available;
    char k;
    void *prev;
    void *next;
} block_t;

/*
* Split the given _block_ to two blocks of lower k
*/
void _split_block(block_t *block);

/*
* Iterate through the available blocks and find one with k equal to _order_
*/
block_t* _find_available_block(char order);

/*
* Coalesce blocks
*/
void _coalesce_blocks(block_t *block);

/*
* Initialise first and last blocks
*/
void _malloc_init(void *malloc_start);

#endif /* _INTERNAL_MALLOC_H */
