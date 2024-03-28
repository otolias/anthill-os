/*
* Memory allocation init and helper functions
*
* Memory allocation uses the buddy system, meaning that the available
* memory pool starts with a single block of size 2 to the power of 12,
* and is then split or coalesced as needed.
*/

#ifndef _INTERNAL_STDLIB_H
#define _INTERNAL_STDLIB_H

#define MALLOC_MAX_ORDER  12
#define MALLOC_BLOCK_SIZE 1 << MALLOC_MAX_ORDER

typedef struct {
    char available;
    char k;
    void *prev;
    void *next;
} block_t;

/*
* Coalesce blocks
*/
void coalesce_blocks(block_t *block);

/*
* Get more memory for the allocator from _block_
*/
short expand(block_t *block);

/*
* Iterate through the available blocks and find one with k equal to _order_
*/
block_t* find_available_block(char order);

/*
 * Release memory from the allocator from _block_
*/
void shrink(block_t *block);

/*
* Split the given _block_ to two blocks of lower k
*/
void split_block(block_t *block);

/*
* Convert _str_ to unsigned long long in radix _base_ and sets _endptr_ to
* point to the rest of the string
*
* Only decimals are currently supported
*
* Return value and failure conditions are described by the strto* functions
*/
unsigned long long strtox(const char *restrict str, char **restrict endptr, int base,
                          unsigned long long limit);

#endif /* _INTERNAL_STDLIB_H */
