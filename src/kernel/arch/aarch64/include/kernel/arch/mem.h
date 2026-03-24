#ifndef _KERNEL_ARCH_AARCH64_MEM_H
#define _KERNEL_ARCH_AARCH64_MEM_H

#include <stdint.h>

#define MEM_LOW  0x80000
#define MEM_HIGH 0x3e000000

#define MEM_VA_KERNEL_START 0xffffffff00000000

#define MEM_RD   (MEM_VA_KERNEL_START + 0x3e000000)
#define MEM_MMIO (MEM_VA_KERNEL_START + 0x3f000000)

#define PAGESIZE 0x1000

/*
* Setup and enable Virtual memory
*/
void mem_init();

#endif /* _KERNEL_ARCH_AARCH64_MEM_H */
