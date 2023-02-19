#ifndef _SYSREGS_H
#define _SYSREGS_H

// Set reserved bits to 1
#define SCTLR_RES   (3 << 28) | (3 << 22) | (1 << 20) | (1 << 11)
// Set explicit data access endianness at EL1 and EL0 to little endian
#define SCTLR_EE    (0 << 25)
#define SCTLR_E0E   (0 << 24)
// Disable instruction cache
#define SCTLR_I     (0 << 12)
// Disable data cache
#define SCTLR_D     (0 << 2)
// Disable MMU
#define SCTLR_MMU   (0 << 0)

#define SCTLR_VALUE (SCTLR_RES | SCTLR_EE | SCTLR_E0E | SCTLR_I | SCTLR_D | SCTLR_MMU)

// Set execution state to AARCH64
#define HCR_VALUE   (1 << 31)

// Disable interrupts
#define SPSR_MASK   (7 << 6)
// Use EL1 dedicated stack pointer
#define SPSR_EL1h   (5 << 0)

#define SPSR_VALUE  (SPSR_MASK | SPSR_EL1h)

// Enable SIMD at EL1
#define CPACR_FPEN  (1 << 20)
#define CPACR_ZEN   (1 << 16)

#define CPACR_VALUE (CPACR_FPEN | CPACR_ZEN)

#endif
