#include "kernel/arch/mem.h"

#include <kernel/assert.h>
#include <kernel/error.h>
#include <kernel/string.h>
#include <kernel/task.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "mmu.h"
#include "pmm.h"

struct mem_r_addr mem_kernel_alloc_page(enum mem_flags flags) {
    // Parse flags
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
            return (struct mem_r_addr) { .addr = NULL, .err = ERR_MEM_FLG };
    }

    // Find empty page
    void *phys_pages[4] = {0};
    phys_pages[3] = pmm_get();
    if (!phys_pages[3])
        return (struct mem_r_addr) { .addr = NULL, .err = ERR_MEM_OOM };

    void *vaddr = MEM_PHYS_TO_VIRT(phys_pages[3]);
    size_t idx[4];
    mmu_get_table_idx(vaddr, idx);

    uintptr_t *levels[4];
    levels[0] = (uintptr_t *) 0xfffffffffffff000;
    levels[1] = mmu_get_next_kernel_level(levels[0], idx[0]);
    levels[2] = mmu_get_next_kernel_level(levels[1], idx[1]);
    levels[3] = mmu_get_next_kernel_level(levels[2], idx[2]);

    // Handle levels 0, 1 and 2
    for (size_t i = 0; i < 3; i++) {
        if (!mmu_is_valid(levels[i][idx[i]])) {
            phys_pages[i] = pmm_get();
            if (!phys_pages[i]) {
                for (size_t p = 0; p < 4; p++) {
                    if (phys_pages[p])
                        pmm_free(phys_pages[p]);
                }

                return (struct mem_r_addr) { .addr = NULL, .err = ERR_MEM_OOM };
            }

            levels[i][idx[i]] = (uintptr_t) phys_pages[i] | ATT_VALID | ATT_PAGE | ATT_AF_SET;
            memset(levels[i + 1], 0, PAGESIZE);
        }
    }

    // Level 3
    if (mmu_is_valid(levels[3][idx[3]])) {
        for (size_t i = 0; i < 4; i++) {
            if (phys_pages[i])
                pmm_free(phys_pages[i]);

            return (struct mem_r_addr) { .addr = NULL, .err = ERR_MEM_EXS };
        }
    }

    levels[3][idx[3]] = 0;
    levels[3][idx[3]] = ((uintptr_t) phys_pages[3]) | attr | ATT_VALID | ATT_PAGE | ATT_NORMAL |
        ATT_AF_SET;

    return (struct mem_r_addr) { .addr = vaddr, .err = ERR_OK };
}

enum kern_err mem_kernel_free_page(void *vaddr) {
    size_t idx[4];
    mmu_get_table_idx(vaddr, idx);

    uintptr_t *levels[4];
    levels[0] = (uintptr_t *) 0xfffffffffffff000;
    levels[1] = mmu_get_next_kernel_level(levels[0], idx[0]);
    levels[2] = mmu_get_next_kernel_level(levels[1], idx[1]);
    levels[3] = mmu_get_next_kernel_level(levels[2], idx[2]);

    for (size_t i = 0; i < 4; i++) {
        if (!mmu_is_valid(levels[i][idx[i]]))
            return ERR_MEM_UNM;

        if (i == 3 || mmu_is_block(levels[i][idx[i]])) {
            assert(i != 0);

            levels[i][idx[i]] = 0;
            void *paddr = MEM_VIRT_TO_PHYS(vaddr);
            size_t shift = PAGESIZE;
            if (i == 1) {
                shift <<= 18;
            } else if (i == 2) {
                shift <<= 9;
            }
            void *paddr_end = paddr + shift;

            for (; paddr < paddr_end; paddr += PAGESIZE)
                pmm_free(paddr);

            break;
        }
    }

    return ERR_OK;
}

void mem_table_soft_copy(void *table) {
    uintptr_t *levels[4] = { table, 0, };

    for (size_t i_0 = 0; i_0 < 512; i_0++) {
        if (!mmu_is_valid(levels[0][i_0]))
            continue;

        levels[1] = mmu_get_vaddr(levels[0][i_0]);

        for (size_t i_1 = 0; i_1 < 512; i_1++) {
            if (!mmu_is_valid(levels[0][i_0]))
                continue;

            mmu_mark_copied(&levels[1][i_1]);

            if (mmu_is_block(levels[1][i_1]))
                continue;

            levels[2] = mmu_get_vaddr(levels[1][i_1]);

            for (size_t i_2 = 0; i_2 < 512; i_2++) {
                if (!mmu_is_valid(levels[2][i_2]))
                    continue;

                mmu_mark_copied(&levels[2][i_2]);

                if (mmu_is_block(levels[2][i_2]))
                    continue;

                levels[3] = mmu_get_vaddr(levels[2][i_2]);

                for (size_t i_3 = 0; i_3 < 512; i_3++) {
                    if (!mmu_is_valid(levels[3][i_3]))
                        continue;

                    mmu_mark_copied(&levels[3][i_3]);
                }
            }
        }
    }
}

enum kern_err mem_table_teardown(void *table) {
    uintptr_t *levels[4] = { table, 0, };
    levels[0] = table;

    for (size_t i_0 = 0; i_0 < 512; i_0++) {
        if (!mmu_is_valid(levels[0][i_0]))
            continue;

        levels[1] = mmu_get_vaddr(levels[0][i_0]);

        for (size_t i_1 = 0; i_1 < 512; i_1++) {
            if (!mmu_is_valid(levels[1][i_1]))
                continue;

            if (mmu_is_block(levels[1][i_1])) {
                uint8_t ref = mmu_mark_freed(&levels[1][i_1]);
                if (ref == 0) {
                    enum kern_err err = mem_kernel_free_page(mmu_get_vaddr(levels[1][i_1]));
                    if (err != ERR_OK)
                        return err;

                    levels[1][i_1] = 0;
                }

                continue;
            }

            levels[2] = mmu_get_vaddr(levels[1][i_1]);

            for (size_t i_2 = 0; i_2 < 512; i_2++) {
                if (!mmu_is_valid(levels[2][i_2]))
                    continue;

                if (mmu_is_block(levels[2][i_2])) {
                    uint8_t ref = mmu_mark_freed(&levels[2][i_2]);
                    if (ref == 0) {
                        enum kern_err err = mem_kernel_free_page(mmu_get_vaddr(levels[2][i_2]));
                        if (err != ERR_OK)
                            return err;

                        levels[2][i_2] = 0;
                    }

                    continue;
                }

                levels[3] = mmu_get_vaddr(levels[2][i_2]);

                for (size_t i_3 = 0; i_3 < 512; i_3++) {
                    if (!mmu_is_valid(levels[3][i_3]))
                        continue;

                    uint8_t ref = mmu_mark_freed(&levels[3][i_3]);
                    if (ref == 0) {
                        enum kern_err err = mem_kernel_free_page(mmu_get_vaddr(levels[3][i_3]));
                        if (err != ERR_OK)
                            return err;

                        levels[3][i_3] = 0;
                    }
                }

                uint8_t ref = mmu_mark_freed(&levels[2][i_2]);
                if (ref == 0) {
                    enum kern_err err = mem_kernel_free_page(mmu_get_vaddr(levels[2][i_2]));
                    if (err != ERR_OK)
                        return err;

                    levels[2][i_2] = 0;
                }
            }

            uint8_t ref = mmu_mark_freed(&levels[1][i_1]);
            if (ref == 0) {
                enum kern_err err = mem_kernel_free_page(mmu_get_vaddr(levels[1][i_1]));
                if (err != ERR_OK)
                    return err;

                levels[1][i_1] = 0;
            }
        }

        uint8_t ref = mmu_mark_freed(&levels[0][i_0]);
        if (ref == 0) {
            enum kern_err err = mem_kernel_free_page(mmu_get_vaddr(levels[0][i_0]));
            if (err != ERR_OK)
                return err;

            levels[0][i_0] = 0;
        }
    }

    return ERR_OK;
}

void* mem_user_find_empty(void *tran_table, void *addr, size_t page_cnt) {
    while (1) {
        bool empty = true;
        size_t p = 0;

        for (p = 0; p < page_cnt; p++) {
            size_t idx[4];
            mmu_get_table_idx(addr + (p * PAGESIZE), idx);

            uintptr_t *level_0 = tran_table;
            if (!mmu_is_valid(level_0[idx[0]]))
                continue;

            uintptr_t *level_1 = mmu_get_vaddr(level_0[idx[0]]);
            if (!mmu_is_valid(level_1[idx[1]]))
                continue;

            if (mmu_is_block(level_1[idx[1]])) {
                empty = false;
                break;
            }

            uintptr_t *level_2 = mmu_get_vaddr(level_1[idx[1]]);
            if (!mmu_is_valid(level_2[idx[2]]))
                continue;

            if (mmu_is_block(level_2[idx[2]])) {
                empty = false;
                break;
            }

            uintptr_t *level_3 = mmu_get_vaddr(level_2[idx[2]]);
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

enum kern_err mem_user_map_page(void *tran_table, void *vaddr, void *paddr, enum mem_flags flags) {
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
            return ERR_MEM_FLG;
    }

    size_t idx[4];
    mmu_get_table_idx(vaddr, idx);

    // Allocated kernel pages
    void *kernel_pages[4] = {0, };

    uintptr_t *levels[4] = { tran_table, 0, };

    // Levels 0, 1 and 2
    for (size_t i = 0; i < 3; i++) {
        if (!mmu_is_valid(levels[i][idx[i]])) {
            struct mem_r_addr page_r = mem_kernel_alloc_page(MEM_RW);
            if (page_r.err != ERR_OK) {
                for (size_t p = 0; p < 4; p++) {
                    if (kernel_pages[p]) {
                        if (mem_kernel_free_page(kernel_pages[p]) != ERR_OK)
                            panic("Tried to de-allocate unmapped kernel page");
                    }
                }

                return page_r.err;
            }

            memset(page_r.addr, 0, PAGESIZE);
            levels[i][idx[i]] = (uintptr_t) MEM_VIRT_TO_PHYS(page_r.addr) |
                ATT_VALID | ATT_PAGE | ATT_AF_SET;
            kernel_pages[i] = page_r.addr;
        }

        levels[i + 1] = mmu_get_vaddr(levels[i][idx[i]]);
    }

    if (mmu_is_valid(levels[3][idx[3]])) {
        for (size_t p = 0; p < 4; p++) {
            if (kernel_pages[p]) {
                if (mem_kernel_free_page(kernel_pages[p]) != ERR_OK)
                    panic("Tried to de-allocate unmapped kernel page");
            }
        }

        return ERR_MEM_EXS;
    }

    levels[3][idx[3]] = (uintptr_t) paddr | attr | ATT_VALID | ATT_PAGE | ATT_NORMAL | ATT_AF_SET;

    return ERR_OK;
}

enum kern_err mem_user_free_page(void *tran_table, void *vaddr) {
    size_t idx[4];
    mmu_get_table_idx(vaddr, idx);
    uintptr_t *levels[4] = { tran_table, 0, };

    if (!mmu_is_valid(levels[0][idx[0]]))
        return ERR_MEM_UNM;

    levels[1] = mmu_get_vaddr(levels[0][idx[0]]);
    if (!mmu_is_valid(levels[1][idx[1]]))
        return ERR_MEM_UNM;

    if (mmu_is_block(levels[1][idx[1]])) {
        uint8_t ref = mmu_mark_freed(&levels[1][idx[1]]);
        if (ref == 0) {
            void *kaddr = mmu_get_vaddr(levels[1][idx[1]]);
            enum kern_err err = mem_kernel_free_page(kaddr);
            if (err != ERR_OK)
                return err;
        }

        levels[1][idx[1]] = 0;
        return ERR_OK;
    }

    levels[2] = mmu_get_vaddr(levels[1][idx[1]]);
    if (!mmu_is_valid(levels[2][idx[2]]))
        return ERR_MEM_UNM;

    if (mmu_is_block(levels[2][idx[2]])) {
        uint8_t ref = mmu_mark_freed(&levels[2][idx[2]]);
        if (ref == 0) {
            void *kaddr = mmu_get_vaddr(levels[2][idx[2]]);
            enum kern_err err = mem_kernel_free_page(kaddr);
            if (err != ERR_OK)
                return err;
        }

        levels[2][idx[2]] = 0;
        return ERR_OK;
    }

    levels[3] = mmu_get_vaddr(levels[2][idx[2]]);
    if (!mmu_is_valid(levels[3][idx[3]]))
        return ERR_MEM_UNM;

    uint8_t ref = mmu_mark_freed(&levels[3][idx[3]]);
    if (ref == 0) {
        void *kaddr = mmu_get_vaddr(levels[3][idx[3]]);
        enum kern_err err = mem_kernel_free_page(kaddr);
        if (err != ERR_OK)
            return err;
    }

    levels[3][idx[3]] = 0;
    return ERR_OK;
}
