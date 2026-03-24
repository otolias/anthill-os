#ifndef _KERNEL_ARCH_AARCH64_MMU_H
#define _KERNEL_ARCH_AARCH64_MMU_H

// 48-bit maximum output address size
#define TCR_IPS   (5 << 32)
// 4 KiB granule size
#define TCR_TGx   (2 << 30 | 0 << 14)
// 32 bits for kernel address space
#define TCR_T1SZ  (32 << 16)
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

#ifndef __ASSEMBLER__

#include <stdint.h>

/*
* Initialise the MMU
*
* _kernel_table_ and _user_table_ are the top level translation tables of
* kernel and user space
*/
void mmu_init(uintptr_t *kernel_table, uintptr_t *user_table);

/*
* Map _vaddr_ to _paddr_.
*
* On success, returns _vaddr_.
* On failure, returns NULL.
*/
uintptr_t mmu_map(uintptr_t vaddr, uintptr_t paddr, int flags);

/**
* Setup kernel translation tables
*
* Returns the address of the top level translation table
*/
uintptr_t* mmu_setup_kernel();

/**
* Setup user translation tables
*
* Returns the address of the top level translation table
*/
uintptr_t* mmu_setup_user();

#endif /* __ASSEMBLER__ */

#endif /* _KERNEL_ARCH_AARCH64_MMU_H */
