#include "kernel/arch/mem.h"

#include "mmu.h"
#include "pmm.h"

#include <kernel/string.h>
#include <kernel/task.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct mem_r_addr mem_alloc_kernel_page(enum mem_flags flags) {
    // Find empty page
    void *page = pmm_get();
    if (!page)
        return (struct mem_r_addr) { .addr = NULL, .err = MEM_ERR_OOM };

    void *vaddr = MEM_PHYS_TO_VIRT(page);

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
            pmm_free(page);
            return (struct mem_r_addr) { .addr = NULL, .err = MEM_ERR_FLG };
    }

    if (mmu_kernel_map(vaddr, page, attr) != vaddr)
        return (struct mem_r_addr) { .addr = NULL, .err = MEM_ERR_OOM };

    return (struct mem_r_addr) { .addr = vaddr, .err = MEM_OK };
}

void mem_free_kernel_page(void *vaddr) {
    mmu_kernel_unmap(vaddr);
}

enum mem_error mem_map_user(void *tran_table, void *vaddr, void *paddr, enum mem_flags flags) {
    uint64_t attr = 0;
    switch (flags) {
        case MEM_RO:
            attr = ATT_AP_RO_RO | ATT_PXN_NOEXEC | ATT_UXN_NOEXEC;
            break;

        case MEM_RW:
            attr = ATT_AP_RW_RW | ATT_PXN_NOEXEC | ATT_UXN_NOEXEC;
            break;

        case MEM_EX:
            attr = ATT_AP_RO_RO | ATT_PXN_NOEXEC | ATT_UXN_EXEC;
            break;

        default:
            return MEM_ERR_FLG;
    }

    if (mmu_user_map(tran_table, vaddr, paddr, attr) != vaddr)
        return MEM_ERR_OOM;

    return MEM_OK;
}

void mem_table_soft_copy(void *table) {
    uintptr_t *level_0 = table;

    for (size_t i_0 = 0; i_0 < 512; i_0++) {
        if ((level_0[i_0] & (1 << ATT_VALID_OFF)) == 0)
            continue;

        uintptr_t *level_1 = MEM_PHYS_TO_VIRT(level_0[i_0] & VADDR_MASK);

        for (size_t i_1 = 0; i_1 < 512; i_1++) {
            if ((level_1[i_1] & (1 << ATT_VALID_OFF)) == 0)
                continue;

            mmu_mark_copied(&level_1[i_1]);

            // If block descriptor, continue
            if ((level_1[i_1] & (1 << ATT_BLOCK_OFF)) == 0)
                continue;

            uintptr_t *level_2 = MEM_PHYS_TO_VIRT(level_1[i_1] & VADDR_MASK);

            for (size_t i_2 = 0; i_2 < 512; i_2++) {
                if ((level_2[i_2] & (1 << ATT_VALID_OFF)) == 0)
                    continue;

                mmu_mark_copied(&level_2[i_2]);

                // If block descriptor, continue
                if ((level_2[i_2] & (1 << ATT_BLOCK_OFF)) == 0)
                    continue;

                uintptr_t *level_3 = MEM_PHYS_TO_VIRT(level_2[i_2] & VADDR_MASK);

                for (size_t i_3 = 0; i_3 < 512; i_3++) {
                    if ((level_3[i_3] & (1 << ATT_VALID_OFF)) == 0)
                        continue;

                    mmu_mark_copied(&level_3[i_3]);
                }
            }
        }
    }
}

void mem_table_teardown(void *table) {
    uintptr_t *level_0 = table;

    for (size_t i_0 = 0; i_0 < 512; i_0++) {
        if ((level_0[i_0] & (1 << ATT_VALID_OFF)) == 0)
            continue;

        uintptr_t *level_1 = MEM_PHYS_TO_VIRT(level_0[i_0] & VADDR_MASK);

        for (size_t i_1 = 0; i_1 < 512; i_1++) {
            if ((level_1[i_1] & (1 << ATT_VALID_OFF)) == 0)
                continue;

            if ((level_1[i_1] & (1 << ATT_BLOCK_OFF)) == 0) {
                // Block descriptor, mark freed and continue
                uint8_t ref = mmu_mark_freed(&level_1[i_1]);
                if (ref == 0) {
                    mmu_kernel_unmap(MEM_PHYS_TO_VIRT(level_1[i_1] & VADDR_MASK));
                    level_1[i_1] = 0;
                }

                continue;
            }

            uintptr_t *level_2 = MEM_PHYS_TO_VIRT(level_1[i_1] & VADDR_MASK);

            for (size_t i_2 = 0; i_2 < 512; i_2++) {
                if ((level_2[i_2] & (1 << ATT_VALID_OFF)) == 0)
                    continue;

                if ((level_2[i_2] & (1 << ATT_BLOCK_OFF)) == 0) {
                    // Block descriptor, mark freed and continue
                    uint8_t ref = mmu_mark_freed(&level_2[i_2]);
                    if (ref == 0) {
                        mmu_kernel_unmap(MEM_PHYS_TO_VIRT(level_2[i_2] & VADDR_MASK));
                        level_2[i_2] = 0;
                    }

                    continue;
                }

                uintptr_t *level_3 = MEM_PHYS_TO_VIRT(level_2[i_2] & VADDR_MASK);

                for (size_t i_3 = 0; i_3 < 512; i_3++) {
                    if ((level_3[i_3] & (1 << ATT_VALID_OFF)) == 0)
                        continue;

                    // Page descriptor, mark freed
                    uint8_t ref = mmu_mark_freed(&level_3[i_3]);
                    // If no more references, unmap from kernel space and free
                    // physical memory
                    if (ref == 0) {
                        mmu_kernel_unmap(MEM_PHYS_TO_VIRT(level_3[i_3] & VADDR_MASK));
                        level_3[i_3] = 0;
                    }
                }

                uint8_t ref = mmu_mark_freed(&level_2[i_2]);
                if (ref == 0) {
                    mmu_kernel_unmap(MEM_PHYS_TO_VIRT(level_2[i_2] & VADDR_MASK));
                    level_2[i_2] = 0;
                }
            }

            uint8_t ref = mmu_mark_freed(&level_1[i_1]);
            if (ref == 0) {
                mmu_kernel_unmap(MEM_PHYS_TO_VIRT(level_1[i_1] & VADDR_MASK));
                level_1[i_1] = 0;
            }
        }

        uint8_t ref = mmu_mark_freed(&level_0[i_0]);
        if (ref == 0) {
            mmu_kernel_unmap(MEM_PHYS_TO_VIRT(level_0[i_0] & VADDR_MASK));
            level_0[i_0] = 0;
        }
    }
}

void* mem_user_find_empty(void *tran_table, void *addr, size_t page_cnt) {
    while (1) {
        bool empty = true;
        size_t p = 0;

        for (p = 0; p < page_cnt; p++) {
            size_t idx[4];
            mmu_get_indices(addr + (p * PAGESIZE), idx);

            uintptr_t *level_0 = tran_table;
            if (!mmu_is_valid(level_0[idx[0]]))
                continue;

            uintptr_t *level_1 = mmu_get_next_level(level_0[idx[0]]);
            if (!mmu_is_valid(level_1[idx[1]]))
                continue;

            if (mmu_is_block(level_1[idx[1]])) {
                empty = false;
                break;
            }

            uintptr_t *level_2 = mmu_get_next_level(level_1[idx[1]]);
            if (!mmu_is_valid(level_2[idx[2]]))
                continue;

            if (mmu_is_block(level_2[idx[2]])) {
                empty = false;
                break;
            }

            uintptr_t *level_3 = mmu_get_next_level(level_2[idx[2]]);
            if (mmu_is_valid(level_3[idx[3]])) {
                empty = false;
                break;
            }
        }

        if (empty)
            break;

        addr += (p + 1) * PAGESIZE;
    }

    return addr;
}

enum mem_error mem_user_unmap_page(void *tran_table, void *vaddr) {
    size_t idx[4];
    mmu_get_indices(vaddr, idx);
    uintptr_t *level_0 = tran_table;

    if (!mmu_is_valid(level_0[idx[0]]))
        return MEM_ERR_UNM;

    uintptr_t *level_1 = mmu_get_next_level(level_0[idx[0]]);
    if (!mmu_is_valid(level_1[idx[1]]))
        return MEM_ERR_UNM;

    // TODO: Handle block

    uintptr_t *level_2 = mmu_get_next_level(level_1[idx[1]]);
    if (!mmu_is_valid(level_2[idx[2]]))
        return MEM_ERR_UNM;

    // TODO: Handle block

    uintptr_t *level_3 = mmu_get_next_level(level_2[idx[2]]);
    if (!mmu_is_valid(level_3[idx[3]]))
        return MEM_ERR_UNM;

    uint8_t ref = mmu_mark_freed(&level_3[idx[3]]);
    if (ref == 0) {
        mmu_kernel_unmap(MEM_PHYS_TO_VIRT(level_3[idx[3]] & VADDR_MASK));
        level_3[idx[3]] = 0;
    }

    return MEM_OK;
}
