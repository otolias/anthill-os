/*
* Memory allocation init and helper functions
*
* Memory allocation uses the buddy system, meaning that the available
* memory pool starts with a single block of size 2 to the power of 12,
* and is then split or coalesced as needed.
*/

#ifndef _INTERNAL_STDLIB_H
#define _INTERNAL_STDLIB_H

#include <sys/types.h>

#define MALLOC_MAX_ORDER  12
#define MALLOC_BLOCK_SIZE 1 << MALLOC_MAX_ORDER

struct block {
    char available;
    char k;
    struct block *prev;
    struct block *next;
};

/*
* Coalesce blocks
*/
void block_coalesce(struct block *block);

/*
* Iterate through the available blocks and find one with k equal to _order_
*/
struct block* block_find(char order);

/*
* Get _ptr_ allocation size in bytes. Returns 0 if block for _ptr_ isn't found.
*/
size_t block_get_size(void *ptr);

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
