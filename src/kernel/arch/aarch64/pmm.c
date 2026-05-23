#include "pmm.h"

#include <kernel/arch/mem.h>
#include <stddef.h>
#include <stdint.h>

#define TOTAL_PAGES ((MEM_HIGH - MEM_LOW) / PAGESIZE)
#define BITS_OF(t) (sizeof(t) * 8)
#define MEM_MAP_SIZE (TOTAL_PAGES / BITS_OF(size_t) - 1)

static size_t memory_map[MEM_MAP_SIZE];

void pmm_free(void *addr) {
    const uintptr_t page = ((uintptr_t) addr - MEM_LOW) / PAGESIZE;
    const size_t index = page / BITS_OF(size_t);
    const size_t offset = page % BITS_OF(size_t);
    memory_map[index] &= ~(1 << offset);
}

void* pmm_get(void) {
    for (size_t i = 0; i < MEM_MAP_SIZE; i++) {
        if (memory_map[i] == SIZE_MAX)
            continue;

        for (size_t off = 0; off < BITS_OF(size_t); off++) {
            if (!(memory_map[i] & 1UL << off)) {
                memory_map[i] |= 1 << off;
                return (void *) MEM_LOW + ((i * BITS_OF(size_t)) + off) * PAGESIZE;
            }
        }
    }

    return NULL;
}

void pmm_mark(const void *addr) {
    const uintptr_t page = ((uintptr_t) addr - MEM_LOW) / PAGESIZE;
    const size_t index = page / BITS_OF(size_t);
    const size_t offset = page % BITS_OF(size_t);
    memory_map[index] |= 1 << offset;
}
