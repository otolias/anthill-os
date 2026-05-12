#include "mmu.h"

#include "mem_map.h"

#include <kernel/arch/mem.h>
#include <kernel/string.h>
#include <stddef.h>
#include <stdint.h>

// Linker addresses
extern volatile char _data_start;
extern volatile char _data_end;

void *mmu_map(uintptr_t vaddr, uintptr_t paddr, uint64_t attr) {
    uintptr_t prefix = vaddr >= MEM_VA_KERNEL_START ? MEM_VA_KERNEL_START : 0;

    const size_t indices[4] = {
        ((vaddr - prefix) >> 39) & 0x1ff,
        ((vaddr - prefix) >> 30) & 0x1ff,
        ((vaddr - prefix) >> 21) & 0x1ff,
        ((vaddr - prefix) >> 12) & 0x1ff,
    };

    uintptr_t *level_0 = (uintptr_t *) (prefix + 0xfffffffff000);
    uintptr_t *level_1 = (uintptr_t *) (((size_t) level_0 << 9) | (indices[0] << 12));
    uintptr_t *level_2 = (uintptr_t *) (((size_t) level_1 << 9) | (indices[1] << 12));
    uintptr_t *level_3 = (uintptr_t *) (((size_t) level_2 << 9) | (indices[2] << 12));

    if ((level_0[indices[0]] & (1 << ATT_VALID_OFF)) == 0) {
        const uintptr_t phys = (uintptr_t) mem_map_get();
        level_0[indices[0]] = phys | ATT_VALID | ATT_PAGE | ATT_AF_SET;
        memset(level_1, 0, PAGESIZE);
    }

    if ((level_1[indices[1]] & (1 << ATT_VALID_OFF)) == 0) {
        const uintptr_t phys = (uintptr_t) mem_map_get();
        level_1[indices[1]] = phys | ATT_VALID | ATT_PAGE | ATT_AF_SET;
        memset(level_2, 0, PAGESIZE);
    }

    if ((level_2[indices[2]] & (1 << ATT_VALID_OFF)) == 0) {
        const uintptr_t phys = (uintptr_t) mem_map_get();
        level_2[indices[2]] = phys | ATT_VALID | ATT_PAGE | ATT_AF_SET;
        memset(level_3, 0, PAGESIZE);
    }

    if ((level_3[indices[3]] & (1 << ATT_VALID_OFF)) != 0)
        return NULL;

    level_3[indices[3]] &= ~0;
    level_3[indices[3]] = paddr | attr | ATT_VALID | ATT_PAGE | ATT_NORMAL | ATT_AF_SET;

    return (void *) vaddr;
}

void mmu_mark_copied(uintptr_t *entry) {
    // Set read-only
    *entry |= ATT_AP_NA_RO;

    // Increment reference counter
    uint64_t ref = (*entry >> ATT_REF_OFF) & 0xf;
    if (ref < ATT_REF_LIMIT)
        *entry |= (++ref << ATT_REF_OFF);
}

void mmu_setup(void) {
    size_t table_cnt = 0;

    // Create kernel page tables
    uintptr_t *kernel_level_0 = (uintptr_t *) &_data_end + (512 * table_cnt++);
    uintptr_t *kernel_level_1 = (uintptr_t *) &_data_end + (512 * table_cnt++);
    uintptr_t *kernel_level_2 = (uintptr_t *) &_data_end + (512 * table_cnt++);
    uintptr_t *kernel_level_3 = (uintptr_t *) &_data_end + (512 * table_cnt++);

    mem_map_mark(kernel_level_0);
    mem_map_mark(kernel_level_1);
    mem_map_mark(kernel_level_2);
    mem_map_mark(kernel_level_3);

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

    mem_map_mark(user_level_0);
    mem_map_mark(user_level_1);
    mem_map_mark(user_level_2);
    mem_map_mark(user_level_3);

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

        mem_map_mark((void *) addr);
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

        mem_map_mark((void *) addr);
    }

    // Map ramdisk
    for (size_t i = 496, block = 0; i < 504; i++, block++) {
        kernel_level_2[i] =
            MEM_RD - MEM_VA_KERNEL_START + block * (PAGESIZE << 9) |
            ATT_VALID | ATT_BLOCK |
            ATT_NORMAL | ATT_AP_NA_RW | ATT_AF_SET |
            ATT_PXN_NOEXEC | ATT_UXN_NOEXEC;

        for (size_t p = 0; p < (PAGESIZE << 9) / PAGESIZE; p++)
            mem_map_mark(MEM_VIRT_TO_PHYS(MEM_RD) + block * p);
    }

    // Map memory mapped peripherals
    for (size_t i = 504, block = 0; i < 512; i++, block++) {
        kernel_level_2[i] =
            MEM_MMIO - MEM_VA_KERNEL_START + block * (PAGESIZE << 9) |
            ATT_VALID | ATT_BLOCK |
            ATT_DEVICE | ATT_AP_NA_RW | ATT_AF_SET |
            ATT_PXN_NOEXEC | ATT_UXN_NOEXEC;

        for (size_t p = 0; p < (PAGESIZE << 9) / PAGESIZE; p++)
            mem_map_mark(MEM_VIRT_TO_PHYS(MEM_MMIO) + block * p);
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
}
