#include "test.h"

#include <kernel/arch/mem.h>
#include <kernel/error.h>
#include <kernel/panic.h>
#include <kernel/task.h>

void _test_run_mem(void) {
    // Allocate kernel page
    struct mem_r_addr kaddr_r = mem_kernel_alloc_page(MEM_RW);
    if (kaddr_r.err != ERR_OK)
        panic("Kernel page allocation failed");

    // Write and read from kernel page
    volatile char *addr = kaddr_r.addr;
    *(addr + 0x100) = 'a';

    if (*(addr + 0x100) != 'a')
        panic("Kernel page access permissions failed");

    // Free kernel page
    if (mem_kernel_free_page(kaddr_r.addr) != ERR_OK)
        panic("Kernel page de-allocation failed");

    // Try to free non-allocated page
    if (mem_kernel_free_page(0) != ERR_MEM_UNM)
        panic("Kernel page invalid de-allocation failed");

    // Re-allocate kernel page
    struct mem_r_addr re_addr_r = mem_kernel_alloc_page(MEM_RW);
    if (re_addr_r.err != ERR_OK || re_addr_r.addr != kaddr_r.addr)
        panic("Kernel page re-allocation failed");

    // Map kernel page to user space
    void *tran_table = MEM_PHYS_TO_VIRT(task_current()->tran_table);
    void *uaddr = (void *) 0x400000;
    void *paddr = MEM_VIRT_TO_PHYS(kaddr_r.addr);
    if (mem_user_map_page(tran_table, uaddr, paddr, MEM_RW) != ERR_OK)
        panic("User page mapping failed");

    // Write and read from user page
    addr = uaddr;
    *(addr + 0x100) = 'a';
    if (*(addr + 0x100) != 'a')
        panic("User page access permissions failed");

    // Find empty space in user space
    void *empty = mem_user_find_empty(tran_table, (void *) 0x3ff000, 2);
    if (empty != (void *) 0x401000)
        panic("Empty user space search failed");

    // Unmap user page
    mem_user_free_page(tran_table, uaddr);
}
