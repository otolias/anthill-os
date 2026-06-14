#ifndef _KERNEL_ARCH_AARCH64_MMU_H
#define _KERNEL_ARCH_AARCH64_MMU_H

#include "sysregs.h"

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

// Access permissions offset
#define ATT_AP_OFF   (6)
// No access in EL0 and read/write in EL1
#define ATT_AP_NA_RW (0 << ATT_AP_OFF)
// Read/write in both EL0 and EL1
#define ATT_AP_RW_RW (1 << ATT_AP_OFF)
// No access in EL0 and read-only in EL1
#define ATT_AP_NA_RO (2 << ATT_AP_OFF)
// Read-only in both EL0 and EL1
#define ATT_AP_RO_RO (3 << ATT_AP_OFF)

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

#define VADDR_MASK 0x0000fffffffff000

#ifndef __ASSEMBLER__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*
* Get translation table indices for virtual address pointed to by _vaddr_ and
* write them to _idx_.
*/
void mmu_get_table_idx(void *vaddr, size_t idx[4]);

/*
* Get the virtual address of the next level entry of _table_ at _idx_.
*/
uintptr_t* mmu_get_next_kernel_level(const uintptr_t *table, size_t idx);

/*
* Get the kernel space virtual address of the physical address in table entry
* _entry_.
*
* Returns a pointer to the virtual address.
*/
uintptr_t* mmu_get_vaddr(uintptr_t entry);

/*
* Handle data abort translation fault for translation table at physical address
* _table_ and virtual address _vaddr_. Doesn't handle invalidation.
*
* On success, returns the start of the virtual address page.
* On failure, returns NULL.
*/
void* mmu_handle_data_abort(uintptr_t table, uintptr_t vaddr);

/*
* Initialise the MMU
*
* _kernel_table_ and _user_table_ are the physical addresses of the top level
* translation tables of kernel and user space respectively.
*/
void mmu_init(void *kernel_table, void *user_table);

/*
* Invalidate translation table cache for virtual address _vaddr_
*/
void mmu_invalidate(uintptr_t vaddr);

/*
* Check if _entry_ is block
*
* Returns true if block, false if not.
*/
bool mmu_is_block(uintptr_t entry);

/*
* Check if _entry_ is valid.
*
* Returns true if valid, false if not.
*/
bool mmu_is_valid(uintptr_t entry);

/*
* Increment reference counter at physical address pointed to by _entry_. If
* it has read/write access in user space, invalidate entry.
*/
void mmu_mark_copied(uintptr_t *entry);

/*
* De-increment reference counter at physical address pointed to by _entry_.
*
* Returns the old value of the reference counter if de-incremented.
*/
uint8_t mmu_mark_freed(uintptr_t *entry);

/*
* Setup translation tables and initialise the MMU.
*
* Returns a pointer to the physical address of the top level user translation
* table.
*/
uintptr_t* mmu_setup(void);

#endif /* __ASSEMBLER__ */
#endif /* _KERNEL_ARCH_AARCH64_MMU_H */
