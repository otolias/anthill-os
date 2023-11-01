#ifndef _MM_H
#define _MM_H

#include <drivers/gpio.h>

#define PAGE_SIZE       4096

#define LOW_MEMORY      PAGE_SIZE + 512
#define HIGH_MEMORY     MMIO_BASE

#define PAGING_PAGES    ((HIGH_MEMORY - LOW_MEMORY) / PAGE_SIZE)

/*
* Find and return consecutive pages to hold _bytes_ bytes.
* If _bytes_ are 0, it returns a single page.
*
* Returns 0 if no consecutive pages are found
*/
void* get_free_pages(unsigned long bytes);
/*
* Deallocate pages starting from page _page_
*/
void free_pages(void *page);

#endif
