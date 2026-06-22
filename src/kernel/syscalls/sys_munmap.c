#include "kernel/syscalls.h"

#include <kernel/arch/mem.h>
#include <kernel/io.h>
#include <kernel/panic.h>
#include <kernel/task.h>

int sys_munmap(void *addr,  size_t len) {
    void *page_start = (void *) ((uintptr_t) addr & ~(PAGESIZE - 1));
    void *page_end = (void *) (((uintptr_t) addr + len) & ~(PAGESIZE - 1));
    void *tran_table = MEM_PHYS_TO_VIRT(task_current()->tran_table);

    for (void *p = page_start; p < page_end; p += PAGESIZE) {
        struct mem_r_addr kaddr_r = mem_user_unmap_page(tran_table, p);
        switch (kaddr_r.err) {
            case ERR_OK:
                if (kaddr_r.addr) {
                    if (mem_kernel_free_page(kaddr_r.addr) != ERR_OK)
                        panic("Tried to free a non allocated kernel page");
                }

            case ERR_MEM_UNM:
                break;

            default:
                io_fmt("Unhandled error code %d\n", kaddr_r.err);
                panic("Unhandled error code");
        }
    }

    return 0;
}
