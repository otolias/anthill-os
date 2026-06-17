#include <kernel/syscalls.h>

#include <kernel/arch/mem.h>
#include <kernel/errno.h>
#include <kernel/io.h>
#include <kernel/panic.h>
#include <kernel/task.h>

void* sys_mmap(void *addr, size_t len, int prot, int flags,
    __attribute__((unused)) int fildes, __attribute__((unused)) off_t off) {
    enum mem_flags m_flags;
    switch (prot) {
        case __PROT_READ:
            m_flags = MEM_RO;
            break;

        case __PROT_READ | __PROT_WRITE:
            m_flags = MEM_RW;
            break;

        case __PROT_READ | __PROT_EXEC:
            m_flags = MEM_EX;
            break;

        default:
            return (void *) ENOTSUP;
    }

    if (flags != __MAP_ANONYMOUS)
        return (void *) ENOTSUP;

    if (addr) {
        // Align up
        addr = (void *) (((uintptr_t) addr + PAGESIZE - 1) & ~(PAGESIZE - 1));
    } else {
        addr = (void *) 0x200000;
    }

    void * const tran_table = MEM_PHYS_TO_VIRT(task_current()->tran_table);
    size_t page_cnt = ((len + PAGESIZE - 1) & ~(PAGESIZE - 1)) / PAGESIZE;

    void * const vaddr = mem_user_find_empty(tran_table, addr, page_cnt);
    if (!vaddr)
        return (void *) ENOMEM;

    for (size_t i = 0; i < page_cnt; i++) {
        // TODO: De-allocate previous if it fails
        struct mem_r_addr page_r = mem_kernel_alloc_page(MEM_RW);
        switch (page_r.err) {
            case ERR_OK:
                break;

            case ERR_MEM_OOM:
                return (void *) ENOMEM;

            default:
                io_fmt("Unhandled error code %d\n", page_r.err);
                panic("Unhandled error code");
        }

        if (mem_user_map_page(
                tran_table,
                vaddr + (i * PAGESIZE),
                MEM_VIRT_TO_PHYS(page_r.addr),
                m_flags
            ) != ERR_OK)
            return (void *) ENOMEM;
    }

    return vaddr;
}
