#ifndef _KERNEL_ARCH_AARCH64_MEM_H
#define _KERNEL_ARCH_AARCH64_MEM_H

#include <stddef.h>
#include <stdint.h>

#define MEM_LOW  0x80000
#define MEM_HIGH 0x3e000000

#define MEM_VA_KERNEL_START 0xffff000000000000
#define MEM_VA_KERNEL_STACK (MEM_VA_KERNEL_START + MEM_LOW - PAGESIZE)

#define MEM_VIRT_TO_PHYS(x) ((void *) (((uintptr_t) (x)) - MEM_VA_KERNEL_START))
#define MEM_PHYS_TO_VIRT(x) ((void *) (((uintptr_t) (x)) + MEM_VA_KERNEL_START))

#define MEM_RD   (MEM_VA_KERNEL_START + 0x3e000000)
#define MEM_MMIO (MEM_VA_KERNEL_START + 0x3f000000)

#define PAGESIZE 0x1000

enum mem_flags {
    MEM_RO = 1, /* Read only permissions */
    MEM_RW = 3, /* Read/write permissions */
    MEM_EX = 5, /* Read and execute permissions */
};

enum mem_error {
    MEM_OK,
    MEM_ERR_OOM, /* Out of memory */
    MEM_ERR_FLG, /* Invalid permission flags */
    MEM_ERR_EXS, /* Tried to map already mapped page */
    MEM_ERR_UNM, /* Tried to unmap already unmapped page */
};

struct mem_r_addr {
    void* addr;
    enum mem_error err;
};

/*
* Allocate kernel page with permissions specified by _flags_.
*
* Returns struct mem_r_addr:
* - On success, _addr_ is the virtual address of the page and _err_ is set to
*   MEM_OK.
* - On failure, _addr_ is NULL and _err_ is set to indicate the error.
*
* Available flags:
*
* Mutually exclusive:
* - MEM_RO Read only permissions
* - MEM_RW Read/write permissions
* - MEM_EX Read and execute permissions
*/
struct mem_r_addr mem_kernel_alloc_page(enum mem_flags flags);

/*
* De-allocate kernel page starting at virtual address pointed to by _vaddr_ and
* its corresponding physical memory.
*
* Returns enum mem_error.
*/
[[nodiscard]] enum mem_error mem_kernel_free_page(void *vaddr);

/*
* Mark all subsequent page and block table entries as copied starting from top
* level table at virtual address pointed to by _table_.
*/
void mem_table_soft_copy(void *table);

/*
* Walk translation level table at virtual address pointed to by _table_ and
* unmark them as copied. Frees their pages if the task is the sole owner.
*
* Returns enum mem_error.
*/
[[nodiscard]] enum mem_error mem_table_teardown(void *table);

/*
* Find unallocated virtual memory to hold _cnt_ pages starting from virtual
* address _addr_ for the translation table at virtual address pointed to by
* _tran_table_.
*
* On success, returns a pointer to the start of the virtual memory found.
* On failure, returns NULL.
*/
void* mem_user_find_empty(void *tran_table, void *addr, size_t page_cnt);

/*
* Map virtual address _vaddr_ to physical address _paddr_ for the translation
* table at virtual address _tran_table_ with permissions specified by _flags_.
*
* Note: _paddr_ must already be allocated by the kernel
*
* Returns enum mem_error
*
* Available flags:
*
* Mutually exclusive:
* - MEM_RO Read only permissions
* - MEM_RW Read/write permissions
* - MEM_EX Read and execute permissions
*/
[[nodiscard]] enum mem_error mem_user_map_page(void *tran_table, void *vaddr, void *paddr,
    enum mem_flags flags);

/*
* Unmap user page starting at virtual address pointed to by _vaddr_ for the
* translation table at the virtual address pointed to by _tran_table_. Frees
* their respective kernel pages if the task is the sole owner.
*
* Returns enum mem_error.
*/
enum mem_error mem_user_free_page(void *tran_table, void *vaddr);

#endif /* _KERNEL_ARCH_AARCH64_MEM_H */
