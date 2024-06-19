#include "kernel/mm.h"

#include <stddef.h>

#define PAGE_FREE   0
#define PAGE_CONS   1
#define PAGE_END    2

static char memory_map[PAGING_PAGES] = {0,};

void* get_free_pages(size_t size) {
    size_t i = 0;
    unsigned long pages_needed = (size / PAGE_SIZE) + 1;

    while (i < PAGING_PAGES) {
        size_t j = i + pages_needed;
        short b = 0;

        if (j > PAGING_PAGES) break;

        while (j-- > i) {
            if (memory_map[j] != PAGE_FREE) {
                i = j + 1;
                b = 1;
                break;
            }
        }

        if (b) continue;

        for (j = 0; j < pages_needed - 1; j++)
            memory_map[i + j] = PAGE_CONS;

        memory_map[i + j] = PAGE_END;

        return (void *) (LOW_MEMORY + i * PAGE_SIZE);
    }

    return NULL;
}

void free_pages(void *page) {
    unsigned long p = (unsigned long) page - LOW_MEMORY;

    while (memory_map[p / PAGE_SIZE] == PAGE_CONS) {
        memory_map[p / PAGE_SIZE] = PAGE_FREE;
        p += PAGE_SIZE;
    }

    memory_map[p / PAGE_SIZE] = PAGE_FREE;
}
