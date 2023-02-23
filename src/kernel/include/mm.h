#ifndef _MM_H
#define _MM_H

#include "gpio.h"
#include <stdint.h>

#define PAGE_SIZE       4096

#define LOW_MEMORY      PAGE_SIZE + 512
#define HIGH_MEMORY     MMIO_BASE

#define PAGING_PAGES    ((HIGH_MEMORY - LOW_MEMORY) / PAGE_SIZE)

/*
* Find and return free page
*/
uintptr_t get_free_page();
/*
* Deallocate page
*/
void free_page(uintptr_t p);

#endif
