#include "mmu.h"

#include <kernel/arch/mem.h>
#include <kernel/string.h>
#include <stdint.h>
#include <stddef.h>

// Valid Descriptor
#define ATT_VALID (1 << 0)
// Descriptor type
#define ATT_BLOCK (0 << 1)
#define ATT_PAGE (1 << 1)

// Device Memory
#define ATT_DEVICE (MAIR_DEVICE_IDX << 2)
// Normal Memory
#define ATT_NORMAL (MAIR_NORMAL_IDX << 2)

// No access in EL0 and read/write in EL1
#define ATT_AP_NA_RW (0 << 6)
// Read/write in both EL0 and EL1
#define ATT_AP_RW_RW (1 << 6)
// No access in EL0 and read-only in EL1
#define ATT_AP_NA_RO (2 << 6)

// Set or unset page access flag
#define ATT_AF_SET   (1 << 10)
#define ATT_AF_UNSET (0 << 10)

// Whether page is executable in EL1
#define ATT_PXN_EXEC   (0L << 53)
#define ATT_PXN_NOEXEC (1L << 53)
// Whether page is executable in EL0
#define ATT_UXN_EXEC   (0L << 54)
#define ATT_UXN_NOEXEC (1L << 54)

// Linker addresses
extern volatile char _data_start;
extern volatile char _data_end;

// Number of translation tables
static size_t table_cnt = 0;

uintptr_t* mmu_setup_kernel() {
    uintptr_t *level_1 = (uintptr_t *) &_data_end + (512 * table_cnt++);
    uintptr_t *level_2 = (uintptr_t *) &_data_end + (512 * table_cnt++);

    memset(level_1, 0, 512 * sizeof(uintptr_t));
    memset(level_2, 0, 512 * sizeof(uintptr_t));

    level_1[0] = (uintptr_t) level_2 | ATT_VALID | ATT_PAGE;

    // Ramdisk
    for (size_t i = 496, block = 0; i < 504; i++, block++) {
        level_2[i] =
            MEM_RD - MEM_VA_KERNEL_START + block * (PAGESIZE << 9) |
            ATT_VALID | ATT_BLOCK |
            ATT_NORMAL | ATT_AP_NA_RW | ATT_AF_SET |
            ATT_PXN_NOEXEC | ATT_UXN_NOEXEC;
    }

    // Memory mapped peripherals
    for (size_t i = 504, block = 0; i < 512; i++, block++) {
        level_2[i] =
            MEM_MMIO - MEM_VA_KERNEL_START + block * (PAGESIZE << 9) |
            ATT_VALID | ATT_BLOCK |
            ATT_DEVICE | ATT_AP_NA_RW | ATT_AF_SET |
            ATT_PXN_NOEXEC | ATT_UXN_NOEXEC;
    }

    return level_1;
}

uintptr_t* mmu_setup_user() {
    uintptr_t *level_0 = (uintptr_t *) &_data_end + (512 * table_cnt++);
    uintptr_t *level_1 = (uintptr_t *) &_data_end + (512 * table_cnt++);
    uintptr_t *level_2 = (uintptr_t *) &_data_end + (512 * table_cnt++);
    uintptr_t *level_3 = (uintptr_t *) &_data_end + (512 * table_cnt++);

    memset(level_0, 0, 512 * sizeof(uintptr_t));
    memset(level_1, 0, 512 * sizeof(uintptr_t));
    memset(level_2, 0, 512 * sizeof(uintptr_t));
    memset(level_3, 0, 512 * sizeof(uintptr_t));

    // Note: This will fail if the kernel segments are over 2 MiB
    level_0[0] = (uintptr_t) level_1 | ATT_VALID | ATT_PAGE;
    level_1[0] = (uintptr_t) level_2 | ATT_VALID | ATT_PAGE;
    level_2[0] = (uintptr_t) level_3 | ATT_VALID | ATT_PAGE;

    // Unused memory
    size_t mapped = MEM_LOW / PAGESIZE - 1;

    // Kernel stack
    level_3[mapped++] =
        MEM_LOW - PAGESIZE |
        ATT_VALID | ATT_PAGE |
        ATT_NORMAL | ATT_AP_NA_RW | ATT_AF_SET |
        ATT_PXN_NOEXEC | ATT_UXN_NOEXEC;

    // Kernel text segment
    const size_t text_pages = ((uintptr_t) &_data_start - MEM_LOW) / PAGESIZE;

    for (size_t i = 0; i < text_pages; i++) {
        level_3[mapped++] =
            MEM_LOW + i * PAGESIZE |
            ATT_VALID | ATT_PAGE |
            ATT_NORMAL | ATT_AP_NA_RO | ATT_AF_SET |
            ATT_PXN_EXEC | ATT_UXN_NOEXEC;
    }

    // Kernel data segment
    const size_t data_pages = (&_data_end - &_data_start) / PAGESIZE;

    for (size_t i = 0; i < data_pages; i++) {
        level_3[mapped++] =
            (uintptr_t) &_data_start + i * PAGESIZE |
            ATT_VALID | ATT_PAGE |
            ATT_NORMAL | ATT_AP_NA_RW | ATT_AF_SET |
            ATT_PXN_NOEXEC | ATT_UXN_NOEXEC;
    }

    // Translation tables
    for (size_t i = 0; i < table_cnt; i++) {
        level_3[mapped++] =
            (uintptr_t) &_data_end + i * PAGESIZE |
            ATT_VALID | ATT_PAGE |
            ATT_NORMAL | ATT_AP_NA_RW | ATT_AF_SET |
            ATT_PXN_NOEXEC | ATT_UXN_NOEXEC;
    }

    return level_0;
}
