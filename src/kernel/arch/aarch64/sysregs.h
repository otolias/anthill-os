#ifndef _KERNEL_ARCH_AARCH64_SYSREGS_H
#define _KERNEL_ARCH_AARCH64_SYSREGS_H

// Enable SIMD at EL1 and EL0
#define CPACR_FPEN (3 << 20)
#define CPACR_ZEN  (3 << 16)

#define CPACR_VAL  (CPACR_FPEN | CPACR_ZEN)

// Exception Class offset
#define ESR_ELx_EC_OFF     26
// SVC Instruction
#define ESR_ELx_EC_SVC64   0x15
// Data Abort
#define ESR_ELx_EC_DT_ABRT 0x24

// Set execution state to AARCH64
#define HCR_VAL (1 << 31)

// Device memory identifier
#define MAIR_DEVICE     (0)
#define MAIR_DEVICE_IDX (0)

// Normal memory identifier
#define MAIR_NORMAL     (1 << 6 | 1 << 2)
#define MAIR_NORMAL_IDX (8)

#define MAIR_VAL        (MAIR_DEVICE << MAIR_DEVICE_IDX | MAIR_NORMAL << MAIR_NORMAL_IDX)

// Reserved values
#define SCTLR_RES      (3 << 28 | 3 << 22 | 1 << 20 | 1 << 11)
// Little-endian data accesses in EL1 and EL0
#define SCTLR_EE       (0 << 25 | 0 << 24)
// Allow caching
#define SCTLR_I        (1 << 12)
// Allow data access caching
#define SCTLR_C        (1 << 2)
// Enable or disable MMU
#define SCTLR_M_SET     (1 << 0)
#define SCTLR_M_UNSET   (0 << 0)

#define SCTLR_MMU_VAL   (SCTLR_RES | SCTLR_EE | SCTLR_I | SCTLR_C | SCTLR_M_SET)

#define SCTLR_NOMMU_VAL (SCTLR_RES | SCTLR_EE | SCTLR_I | SCTLR_C | SCTLR_M_UNSET)

/* Disable interrupts */
#define SPSR_MASK (7 << 6)
/* Use EL1 dedicated stack pointer */
#define SPSR_EL1h (5 << 0)

#define SPSR_VAL  (SPSR_MASK | SPSR_EL1h)

// 48-bit maximum output address size
#define TCR_IPS  (5 << 32)
// 4 KiB granule size
#define TCR_TGx  (2 << 30 | 0 << 14)
// 48 bits for kernel address space
#define TCR_T1SZ (16 << 16)
// 48 bits for user address space
#define TCR_T0SZ (16 << 0)

#define TCR_VAL  (TCR_IPS | TCR_TGx | TCR_T1SZ | TCR_T0SZ)

#endif /* _KERNEL_ARCH_AARCH64_SYSREGS_H */
