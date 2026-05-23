#include "kernel/elf.h"

#include <kernel/arch/mem.h>
#include <kernel/string.h>
#include <kernel/sys/types.h>
#include <stdint.h>

#define ELF_OFF(base, offset) ((void *) (((size_t) (base)) + (offset)))

struct elf_r_addr elf_create_proc_image(const struct elf64_ehdr *ehdr, void *tran_table) {
    const struct elf64_phdr *phdr = (struct elf64_phdr *) ((char *) ehdr + ehdr->e_phoff);
    void *base_address = (void *) (phdr->p_vaddr - phdr->p_offset);

    for (size_t i = 0; i < ehdr->e_phnum; i++, phdr++) {
        if (phdr->p_type != PT_LOAD)
            continue;

        // Parse flags
        enum mem_flags flags = 0;

        switch (phdr->p_flags) {
            case PF_R:
                flags = MEM_RO;
                break;

            case PF_R | PF_X:
                flags = MEM_EX;
                break;

            case PF_R | PF_W:
                flags = MEM_RW;
                break;

            default:
                return (struct elf_r_addr) { .addr = NULL, .err = ELF_ERR_INV };
        }

        // Allocate pages for the segments
        uintptr_t vaddr = phdr->p_vaddr & ~(PAGESIZE - 1);
        uintptr_t vaddr_end = phdr->p_vaddr + phdr->p_filesz;

        while (vaddr < vaddr_end) {
            // Allocate page
            const struct mem_r_addr page_r = mem_alloc_kernel_page(MEM_RW);
            if (page_r.err != MEM_OK) {
                // TODO: Free previous pages
                return (struct elf_r_addr) { .addr = NULL, .err = ELF_ERR_OOM };
            }

            // Map page to user space
            if (mem_map_user(tran_table, (void *) vaddr, MEM_VIRT_TO_PHYS(page_r.addr), flags)
                != MEM_OK) {
                // TODO: Free page_r
                return (struct elf_r_addr) { .addr = NULL, .err = ELF_ERR_OOM };
            }

            uintptr_t data_start = vaddr > phdr->p_vaddr ? vaddr : phdr->p_vaddr;
            uintptr_t data_end = vaddr + PAGESIZE < vaddr_end ? vaddr + PAGESIZE : vaddr_end;
            uintptr_t file_offset = phdr->p_offset + data_start - phdr->p_vaddr;
            uintptr_t page_offset = data_start & (PAGESIZE - 1);

            memcpy(
                ((char *) page_r.addr + page_offset),
                ((char *) ehdr) + file_offset,
                data_end - data_start
            );

            vaddr += PAGESIZE;
        }
    }

    return (struct elf_r_addr) { .addr = (void *) (base_address - PAGESIZE), .err = ELF_OK };
}

enum elf_err elf_validate(const struct elf64_ehdr *ehdr) {
    if (memcmp(ehdr->e_ident, "\x7f""ELF", 4) != 0)
        return ELF_ERR_INV;

    if (ehdr->e_type != ET_EXEC)
        return ELF_ERR_UNS;

    if (ehdr->e_machine != 0xb7)
        return ELF_ERR_MAC;

    return ELF_OK;
}
