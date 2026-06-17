#include "mmu.h"

#include "pmm.h"

#include <kernel/arch/mem.h>
#include <kernel/string.h>
#include <stddef.h>
#include <stdint.h>

// Linker addresses
extern volatile char _data_start;
extern volatile char _data_end;

void mmu_get_table_idx(void *vaddr, size_t idx[4]) {
    idx[0] = ((uintptr_t) vaddr >> 39) & 0x1ff;
    idx[1] = ((uintptr_t) vaddr >> 30) & 0x1ff;
    idx[2] = ((uintptr_t) vaddr >> 21) & 0x1ff;
    idx[3] = ((uintptr_t) vaddr >> 12) & 0x1ff;
}

uintptr_t* mmu_get_next_kernel_level(const uintptr_t *table, size_t idx) {
    return (uintptr_t *) (((uintptr_t) table << 9 | (idx << 12)));
}

uintptr_t* mmu_get_vaddr(uintptr_t entry) {
    return MEM_PHYS_TO_VIRT(entry & VADDR_MASK);
}

void* mmu_handle_data_abort(uintptr_t table, uintptr_t vaddr) {
    size_t idx[4];
    mmu_get_table_idx((void *) vaddr, idx);

    uint8_t ref;

    uintptr_t *level_0 = MEM_PHYS_TO_VIRT(table);
    if ((level_0[idx[0]] & (1 << ATT_VALID_OFF)) == 0)
        return NULL;

    uintptr_t *level_1 = MEM_PHYS_TO_VIRT(level_0[idx[0]] & VADDR_MASK);
    if ((level_1[idx[1]] & (1 << ATT_VALID_OFF)) == 0)
        return NULL;

    // TODO: Handle block

    ref = mmu_mark_freed(&level_1[idx[1]]);
    if (ref > 0) {
        struct mem_r_addr page_r = mem_kernel_alloc_page(MEM_RW);
        if (page_r.err != ERR_OK)
            return NULL;

        memcpy(page_r.addr, level_1, PAGESIZE);

        level_1 = page_r.addr;

        level_0[idx[0]] = (uintptr_t) MEM_VIRT_TO_PHYS(page_r.addr) |
            ATT_VALID | ATT_PAGE | ATT_AF_SET;
    }

    uintptr_t *level_2 = MEM_PHYS_TO_VIRT(level_1[idx[1]] & VADDR_MASK);
    if ((level_2[idx[2]] & (1 << ATT_VALID_OFF)) == 0)
        return NULL;

    // TODO: Handle block

    ref = mmu_mark_freed(&level_2[idx[2]]);
    if (ref > 0) {
        struct mem_r_addr page_r = mem_kernel_alloc_page(MEM_RW);
        if (page_r.err != ERR_OK)
            return NULL;

        memcpy(page_r.addr, level_2, PAGESIZE);

        level_2 = page_r.addr;

        level_1[idx[1]] = (uintptr_t) MEM_VIRT_TO_PHYS(page_r.addr) |
            ATT_VALID | ATT_PAGE | ATT_AF_SET;
    }

    uintptr_t *level_3 = MEM_PHYS_TO_VIRT(level_2[idx[2]] & VADDR_MASK);
    if ((level_3[idx[3]] & (1 << ATT_VALID_OFF)) == 0)
        return NULL;

    ref = mmu_mark_freed(&level_3[idx[3]]);
    if (ref > 0) {
        struct mem_r_addr page_r = mem_kernel_alloc_page(MEM_RW);
        if (page_r.err != ERR_OK)
            return NULL;

        memcpy(page_r.addr, level_3, PAGESIZE);

        level_3 = page_r.addr;

        level_2[idx[2]] = (uintptr_t) MEM_VIRT_TO_PHYS(page_r.addr) |
            ATT_VALID | ATT_PAGE | ATT_AF_SET;
    }

    struct mem_r_addr page_r = mem_kernel_alloc_page(MEM_RW);
    if (page_r.err != ERR_OK)
        return NULL;

    const void *data = MEM_PHYS_TO_VIRT(level_3[idx[3]] & VADDR_MASK);
    memcpy(page_r.addr, data, PAGESIZE);

    level_3[idx[3]] &= ~VADDR_MASK;
    level_3[idx[3]] &= ~(0x3 << ATT_AP_OFF);
    level_3[idx[3]] |= (uintptr_t) MEM_VIRT_TO_PHYS(page_r.addr) | ATT_AP_RW_RW;

    return (void *) (vaddr & VADDR_MASK);
}

bool mmu_is_block(uintptr_t entry) {
    return (entry & (1 << ATT_BLOCK_OFF)) == 0;
}

bool mmu_is_valid(uintptr_t entry) {
    return (entry & (1 << ATT_VALID_OFF)) != 0;
}

void mmu_mark_copied(uintptr_t *entry) {
    // Increment reference counter
    uint64_t ref = (*entry >> ATT_REF_OFF) & 0xf;
    if (ref < ATT_REF_LIMIT) {
        *entry &= ~(0xfUL << ATT_REF_OFF);
        *entry |= ((ref + 1) << ATT_REF_OFF);
    }

    // If read/write in user space, set to read-only in kernel space
    if ((*entry & (3 << ATT_AP_OFF)) == ATT_AP_RW_RW) {
        *entry &= ~(3 << ATT_AP_OFF);
        *entry |= ATT_AP_NA_RO;
    }
}

uint8_t mmu_mark_freed(uintptr_t *entry) {
    uint64_t ref = (*entry >> ATT_REF_OFF) & 0xf;
    if (ref > 0) {
        *entry &= ~(0xfUL << ATT_REF_OFF);
        *entry |= ((ref - 1) << ATT_REF_OFF);
    }

    return (uint8_t) ref;
}

uintptr_t* mmu_setup(void) {
    size_t table_cnt = 0;

    // Create kernel page tables
    uintptr_t *kernel_level_0 = (uintptr_t *) &_data_end + (512 * table_cnt++);
    uintptr_t *kernel_level_1 = (uintptr_t *) &_data_end + (512 * table_cnt++);
    uintptr_t *kernel_level_2 = (uintptr_t *) &_data_end + (512 * table_cnt++);
    uintptr_t *kernel_level_3 = (uintptr_t *) &_data_end + (512 * table_cnt++);

    pmm_mark(kernel_level_0);
    pmm_mark(kernel_level_1);
    pmm_mark(kernel_level_2);
    pmm_mark(kernel_level_3);

    memset(kernel_level_0, 0, PAGESIZE);
    memset(kernel_level_1, 0, PAGESIZE);
    memset(kernel_level_2, 0, PAGESIZE);
    memset(kernel_level_3, 0, PAGESIZE);

    // Setup recursive page table mapping
    kernel_level_0[511] = (uintptr_t) kernel_level_0 | ATT_VALID | ATT_PAGE | ATT_AF_SET;

    kernel_level_0[0] = (uintptr_t) kernel_level_1 | ATT_VALID | ATT_PAGE | ATT_AF_SET;
    kernel_level_1[0] = (uintptr_t) kernel_level_2 | ATT_VALID | ATT_PAGE | ATT_AF_SET;
    kernel_level_2[0] = (uintptr_t) kernel_level_3 | ATT_VALID | ATT_PAGE | ATT_AF_SET;

    // Create user page tables
    uintptr_t *user_level_0 = (uintptr_t *) &_data_end + (512 * table_cnt++);
    uintptr_t *user_level_1 = (uintptr_t *) &_data_end + (512 * table_cnt++);
    uintptr_t *user_level_2 = (uintptr_t *) &_data_end + (512 * table_cnt++);
    uintptr_t *user_level_3 = (uintptr_t *) &_data_end + (512 * table_cnt++);

    pmm_mark(user_level_0);
    pmm_mark(user_level_1);
    pmm_mark(user_level_2);
    pmm_mark(user_level_3);

    memset(user_level_0, 0, PAGESIZE);
    memset(user_level_1, 0, PAGESIZE);
    memset(user_level_2, 0, PAGESIZE);
    memset(user_level_3, 0, PAGESIZE);

    // Note: This will fail if the kernel segments are over 2 MiB
    user_level_0[0] = (uintptr_t) user_level_1 | ATT_VALID | ATT_PAGE;
    user_level_1[0] = (uintptr_t) user_level_2 | ATT_VALID | ATT_PAGE;
    user_level_2[0] = (uintptr_t) user_level_3 | ATT_VALID | ATT_PAGE;

    // Skip unused memory
    size_t mapped = MEM_LOW / PAGESIZE - 1;

    // Map kernel stack
    const uintptr_t addr = MEM_LOW - PAGESIZE;

    kernel_level_3[mapped] = addr |
        ATT_VALID | ATT_PAGE |
        ATT_NORMAL | ATT_AP_NA_RW | ATT_AF_SET |
        ATT_PXN_NOEXEC | ATT_UXN_NOEXEC;

    user_level_3[mapped++] = addr |
        ATT_VALID | ATT_PAGE |
        ATT_NORMAL | ATT_AP_NA_RW | ATT_AF_SET |
        ATT_PXN_NOEXEC | ATT_UXN_NOEXEC;

    // Map kernel text segment
    const size_t text_pages = ((uintptr_t) &_data_start - MEM_LOW) / PAGESIZE;

    for (size_t i = 0; i < text_pages; i++, mapped++) {
        const uintptr_t addr = MEM_LOW + i * PAGESIZE;

        kernel_level_3[mapped] = addr |
            ATT_VALID | ATT_PAGE |
            ATT_NORMAL | ATT_AP_NA_RO | ATT_AF_SET |
            ATT_PXN_EXEC | ATT_UXN_NOEXEC;

        user_level_3[mapped] = addr |
            ATT_VALID | ATT_PAGE |
            ATT_NORMAL | ATT_AP_NA_RO | ATT_AF_SET |
            ATT_PXN_EXEC | ATT_UXN_NOEXEC;

        pmm_mark((void *) addr);
    }

    // Map kernel data segment
    const size_t data_pages = ((uintptr_t) &_data_end - (uintptr_t) &_data_start) / PAGESIZE;

    for (size_t i = 0; i < data_pages; i++, mapped++) {
        const uintptr_t addr = (uintptr_t) &_data_start + i * PAGESIZE;

        kernel_level_3[mapped] = addr |
            ATT_VALID | ATT_PAGE |
            ATT_NORMAL | ATT_AP_NA_RW | ATT_AF_SET |
            ATT_PXN_NOEXEC | ATT_UXN_NOEXEC;

        user_level_3[mapped] = addr |
            ATT_VALID | ATT_PAGE |
            ATT_NORMAL | ATT_AP_NA_RW | ATT_AF_SET |
            ATT_PXN_NOEXEC | ATT_UXN_NOEXEC;

        pmm_mark((void *) addr);
    }

    // Map ramdisk
    for (size_t i = 496, block = 0; i < 504; i++, block++) {
        kernel_level_2[i] =
            MEM_RD - MEM_VA_KERNEL_START + block * (PAGESIZE << 9) |
            ATT_VALID | ATT_BLOCK |
            ATT_NORMAL | ATT_AP_NA_RW | ATT_AF_SET |
            ATT_PXN_NOEXEC | ATT_UXN_NOEXEC;

        for (size_t p = 0; p < (PAGESIZE << 9) / PAGESIZE; p++)
            pmm_mark(MEM_VIRT_TO_PHYS(MEM_RD) + block * p);
    }

    // Map memory mapped peripherals
    for (size_t i = 504, block = 0; i < 512; i++, block++) {
        kernel_level_2[i] =
            MEM_MMIO - MEM_VA_KERNEL_START + block * (PAGESIZE << 9) |
            ATT_VALID | ATT_BLOCK |
            ATT_DEVICE | ATT_AP_NA_RW | ATT_AF_SET |
            ATT_PXN_NOEXEC | ATT_UXN_NOEXEC;

        for (size_t p = 0; p < (PAGESIZE << 9) / PAGESIZE; p++)
            pmm_mark(MEM_VIRT_TO_PHYS(MEM_MMIO) + block * p);
    }

    // Map these translation tables
    for (size_t i = 0; i < table_cnt; i++) {
        kernel_level_3[mapped++] =
            (uintptr_t) &_data_end + i * PAGESIZE |
            ATT_VALID | ATT_PAGE |
            ATT_NORMAL | ATT_AP_NA_RW | ATT_AF_SET |
            ATT_PXN_NOEXEC | ATT_UXN_NOEXEC;
    }

    mmu_init(kernel_level_0, user_level_0);

    return user_level_0;
}
