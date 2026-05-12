#ifndef _KERNEL_ARCH_AARCH64_MEM_MAP_H
#define _KERNEL_ARCH_AARCH64_MEM_MAP_H

/*
* Mark _addr_ as free in memory map
*/
void mem_map_free(const void *addr);

/*
* Get empty page from memory map
*
* On success, marks page and returns a pointer to the start of the found page
* On failure, returns NULL
*/
void* mem_map_get(void);

/*
* Mark _addr_ as occupied in memory map
*/
void mem_map_mark(const void *addr);

#endif /* _KERNEL_ARCH_AARCH64_MEM_MAP_H */
