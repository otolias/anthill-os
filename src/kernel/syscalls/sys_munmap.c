#include "kernel/syscalls.h"

#include <kernel/arch/mem.h>
#include <kernel/task.h>

int sys_munmap(void *addr,  size_t len) {
    void *page_start = (void *) ((uintptr_t) addr & ~(PAGESIZE - 1));
    void *page_end = (void *) (((uintptr_t) addr + len) & ~(PAGESIZE - 1));

    for (void *p = page_start; p < page_end; p += PAGESIZE)
        mem_user_free_page(MEM_PHYS_TO_VIRT(task_current()->tran_table), p);

    return 0;
}
