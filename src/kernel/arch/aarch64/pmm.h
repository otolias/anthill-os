#ifndef _KERNEL_ARCH_AARCH64_PMM_H
#define _KERNEL_ARCH_AARCH64_PMM_H

/*
* Mark _addr_ as free in the physical memory map.
*/
void pmm_free(void *addr);

/*
* Get empty page from the physical memory map.
*
* On success, marks page and returns the physical address of the page.
* On failure, returns NULL.
*/
void* pmm_get(void);

/*
* Mark _addr_ as occupied in the physical memory map
*/
void pmm_mark(const void *addr);

#endif /* _KERNEL_ARCH_AARCH64_PMM_H */
