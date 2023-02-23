#include "mm.h"
#include <stdint.h>

static uint8_t memory_map [PAGING_PAGES] = {0,};

uintptr_t get_free_page() {
    for (int i = 0; i < PAGING_PAGES; i++) {
        if (memory_map[i] == 0) {
            memory_map[i] = 1;
            return LOW_MEMORY + i * PAGE_SIZE;
        }
    }

    return 0;
}

void free_page(uintptr_t p) {
    memory_map[(p - LOW_MEMORY) / PAGE_SIZE] = 0;
}
