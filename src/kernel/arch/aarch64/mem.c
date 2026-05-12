#include "kernel/arch/mem.h"

#include "mem_map.h"
#include "mmu.h"

#include <kernel/string.h>
#include <kernel/sysregs.h>
#include <kernel/task.h>
#include <stddef.h>
#include <stdint.h>

#define TOTAL_PAGES ((MEM_HIGH - MEM_LOW) / PAGESIZE)

struct mem_r_addr mem_get_kernel_page(int flags) {
    // Find empty page
    const void *page = mem_map_get();
    if (!page)
        return (struct mem_r_addr) { .addr = NULL, .err = MEM_ERR_OOM };

    const uintptr_t paddr = (uintptr_t) page;
    const uintptr_t vaddr = (uintptr_t) MEM_PHYS_TO_VIRT(paddr);

    uint64_t attr = 0;
    switch (flags) {
        case MEM_RO:
            attr = ATT_AP_NA_RO | ATT_PXN_NOEXEC | ATT_UXN_NOEXEC;
            break;
        case MEM_RW:
            attr = ATT_AP_NA_RW | ATT_PXN_NOEXEC | ATT_UXN_NOEXEC;
            break;
        case MEM_EX:
            attr = ATT_AP_NA_RO | ATT_PXN_EXEC | ATT_UXN_NOEXEC;
            break;
        default:
            mem_map_free(page);
            return (struct mem_r_addr) { .addr = NULL, .err = MEM_ERR_INV };
    }

    if (mmu_map(vaddr, paddr, attr) != (void *) vaddr)
        return (struct mem_r_addr) { .addr = NULL, MEM_ERR_OOM };

    return (struct mem_r_addr) { .addr = (void *) vaddr, .err = MEM_OK };
}

void mem_mark_copied(const uintptr_t *table) {
    for (size_t i_0 = 0; i_0 < 512; i_0++) {
        if ((table[i_0] & (1 << ATT_VALID_OFF)) == 0)
            continue;

        uintptr_t *level_1 = MEM_PHYS_TO_VIRT(table[i_0] & ~(0xfff));

        for (size_t i_1 = 0; i_1 < 512; i_1++) {
            if ((level_1[i_1] & (1 << ATT_BLOCK_OFF)) == 0) {
                // Mark as read-only
                mmu_mark_copied(&level_1[i_1]);
                continue;
            }

            uintptr_t *level_2 = MEM_PHYS_TO_VIRT(level_1[i_1] & ~(0xfff));

            for (size_t i_2 = 0; i_2 < 512; i_2++) {
                if ((level_2[i_2] & (1 << ATT_VALID_OFF)) == 0)
                    continue;

                if ((level_2[i_2] & (1 << ATT_BLOCK_OFF)) == 0) {
                    mmu_mark_copied(&level_2[i_2]);
                    continue;
                }

                uintptr_t *level_3 = MEM_PHYS_TO_VIRT(level_2[i_2] & ~(0xfff));

                for (size_t i_3 = 0; i_3 < 512; i_3++) {
                    if ((level_3[i_3] & (1 << ATT_VALID_OFF)) == 0)
                        continue;

                    mmu_mark_copied(&level_3[i_3]);
                }
            }
        }
    }
}
