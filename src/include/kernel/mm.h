#ifndef _KERNEL_MM_H
#define _KERNEL_MM_H

#include <stddef.h>

#include <drivers/gpio.h>

#define PAGE_SIZE     4096

#define LOW_MEMORY    0x80000
#define HIGH_MEMORY   MMIO_BASE

#define PAGING_PAGES ((HIGH_MEMORY - LOW_MEMORY) / PAGE_SIZE)

/*
* Deallocate pages starting from page _page_
*/
void mm_free_pages(void *page);

/*
* Find and return consecutive pages to hold _size_ bytes.
* If _size_ is 0, returns a single page.
*
* On success, returns a pointer to the allocated space.
* On failure, returns a null pointer.
*/
void* mm_get_pages(size_t size);

/*
* Mark kernel pages
*/
void mm_init(void);

#endif /* _KERNEL_MM_H */
