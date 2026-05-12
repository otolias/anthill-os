#ifndef _KERNEL_ARCH_AARCH64_MEM_H
#define _KERNEL_ARCH_AARCH64_MEM_H

#include <stddef.h>
#include <stdint.h>

#define MEM_LOW  0x80000
#define MEM_HIGH 0x3e000000

#define MEM_RO (1)
#define MEM_RW (MEM_RO | 1 << 1)
#define MEM_EX (MEM_RO | 1 << 2)

#define MEM_VA_KERNEL_START 0xffff000000000000
#define MEM_VA_KERNEL_STACK MEM_VA_KERNEL_START + MEM_LOW - PAGESIZE

#define MEM_VIRT_TO_PHYS(x) ((void *) (x)) - MEM_VA_KERNEL_START
#define MEM_PHYS_TO_VIRT(x) ((void *) (x)) + MEM_VA_KERNEL_START

#define MEM_RD   MEM_VA_KERNEL_START + 0x3e000000
#define MEM_MMIO MEM_VA_KERNEL_START + 0x3f000000

#define PAGESIZE 0x1000

enum mem_error {
    MEM_OK,
    MEM_ERR_OOM, /* Out of memory */
    MEM_ERR_INV, /* Invalid permission flags */
    MEM_ERR_UNK, /* Unknown/unimplemented data abort */
};

struct mem_r_addr {
    void* addr;
    enum mem_error err;
};

/*
* Allocate and return kernel page with permissions specified by _flags_.
*
* Returns struct mem_r_page:
*   On success, _addr_ is the virtual address of the page and _err_ is MEM_OK.
*   On failure, _addr_ is NULL and _err_ is set to indicate the error.
*
* Available flags:
*
* Mutually exclusive:
* - MEM_RO Read only permissions
* - MEM_RW Read/write permissions
* - MEM_EX Read and execute permissions
*/
struct mem_r_addr mem_get_kernel_page(int flags);

/*
* Mark all page and block table entries as copied starting from top level table
* _table_.
*/
void mem_mark_copied(const uintptr_t *table);

#endif /* _KERNEL_ARCH_AARCH64_MEM_H */
