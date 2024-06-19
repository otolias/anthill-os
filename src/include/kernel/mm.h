#ifndef _MM_H
#define _MM_H

#include <stddef.h>

#include <drivers/gpio.h>

#define PAGE_SIZE       4096

#define LOW_MEMORY      PAGE_SIZE
#define HIGH_MEMORY     MMIO_BASE

#define PAGING_PAGES    ((HIGH_MEMORY - LOW_MEMORY) / PAGE_SIZE)

/*
* Find and return consecutive pages to hold _size_ bytes.
* If _size_ is 0, returns a single page.
*
* On success, returns a pointer to the allocated space.
* On failure, returns a null pointer.
*/
void* get_free_pages(size_t size);

/*
* Deallocate pages starting from page _page_
*/
void free_pages(void *page);

#endif
