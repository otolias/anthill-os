#ifndef _KERNEL_ARCH_AARCH64_MMU_H
#define _KERNEL_ARCH_AARCH64_MMU_H

/* === System registers === */

// 48-bit maximum output address size
#define TCR_IPS   (5 << 32)
// 4 KiB granule size
#define TCR_TGx   (2 << 30 | 0 << 14)
// 48 bits for kernel address space
#define TCR_T1SZ  (16 << 16)
// 48 bits for user address space
#define TCR_T0SZ  (16 << 0)

#define TCR_VALUE (TCR_IPS | TCR_TGx | TCR_T1SZ | TCR_T0SZ)

// Device memory identifier
#define MAIR_DEVICE     (0)
#define MAIR_DEVICE_IDX (0)

// Normal memory identifier
#define MAIR_NORMAL     (1 << 6 | 1 << 2)
#define MAIR_NORMAL_IDX (8)

#define MAIR_VALUE      (MAIR_DEVICE << MAIR_DEVICE_IDX | MAIR_NORMAL << MAIR_NORMAL_IDX)

// Reserved values
#define SCTLR_RES   (3 << 28 | 3 << 22 | 1 << 20 | 1 << 11)
// Little-endian data accesses in EL1 and EL0
#define SCTLR_EE    (0 << 25 | 0 << 24)
// Allow caching
#define SCTLR_I     (1 << 12)
// Enable alignment checks in EL1 and EL0
#define SCTLR_SA    (3 << 3)
// Allow data access caching
#define SCTLR_C     (1 << 2)
// Enable execution alignment check
#define SCTLR_A     (1 << 1)
// Enable MMU
#define SCTLR_M     (1 << 0)

#define SCTLR_VALUE (SCTLR_RES | SCTLR_EE | SCTLR_I | SCTLR_SA | SCTLR_C | SCTLR_A | SCTLR_M)

/* === Translation table entry attributes === */

// Valid Descriptor
#define ATT_VALID_OFF (0)
#define ATT_VALID     (1 << ATT_VALID_OFF)
// Descriptor type
#define ATT_BLOCK_OFF (1)
#define ATT_BLOCK     (0 << ATT_BLOCK_OFF)
#define ATT_PAGE_OFF  (1)
#define ATT_PAGE      (1 << ATT_PAGE_OFF)

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
// Read-only in both EL0 and EL1
#define ATT_AP_RO_RO (3 << 6)

// Set or unset page access flag
#define ATT_AF_SET   (1 << 10)
#define ATT_AF_UNSET (0 << 10)

// Whether page is executable in EL1
#define ATT_PXN_EXEC   (0L << 53)
#define ATT_PXN_NOEXEC (1L << 53)
// Whether page is executable in EL0
#define ATT_UXN_EXEC   (0L << 54)
#define ATT_UXN_NOEXEC (1L << 54)

// Reference counter shift position
#define ATT_REF_OFF  55 // - 59
// Reference counter limit
#define ATT_REF_LIMIT 0xf

#ifndef __ASSEMBLER__

#include <stddef.h>
#include <stdint.h>

/*
* Initialise the MMU
*
* _kernel_table_ and _user_table_ are the physical addresses of the top level
* translation tables of kernel and user space respectively.
*/
void mmu_init(void *kernel_table, void *user_table);

/*
* Map virtual address _vaddr_ to physical address _paddr_ with access attributes
* specified by _attr_.
*
* On success, returns _vaddr_.
* On failure, returns NULL.
*/
[[nodiscard]] void* mmu_kernel_map(void *vaddr, const void *paddr, uint64_t attr);

/*
* Unmap virtual address pointed to by _vaddr_ from kernel space and
* deallocate the corresponding physical memory
*/
void mmu_kernel_unmap(void *vaddr);

/*
* Set read-only permissions to table entry at physical address pointed to by
* _entry_ and increment reference counter.
*/
void mmu_mark_copied(uintptr_t *entry);

/*
* De-increment reference counter of table entry at physical address pointed to
* by _entry_.
*
* Returns the reference counter.
*/
uint8_t mmu_mark_freed(uintptr_t *entry);

/*
* Setup translation tables and initialise the MMU.
*/
void mmu_setup(void);

/*
* Map virtual address _vaddr_ to physical address _paddr_ for the translation
* table at virtual address _tran_table_ with access attributes specified by
* _attr_.
*
* On success, returns _vaddr_.
* On failure, returns NULL.
*/
[[nodiscard]] void* mmu_user_map(void *tran_table, void *vaddr, const void *paddr, uint64_t attr);

#endif /* __ASSEMBLER__ */

#endif /* _KERNEL_ARCH_AARCH64_MMU_H */
