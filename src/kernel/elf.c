#include <kernel/elf.h>

#include <kernel/string.h>

short elf_validate(const Elf64_Ehdr *ehdr) {
    if (strncmp((char *) ehdr->e_ident, "\x7f""ELF", 4))
        return 0;

    if (ehdr->e_type != 3)
        return 0;

    if (ehdr->e_machine != 0xb7)
        return 0;

    return 1;
}

unsigned long elf_get_image_size(const Elf64_Ehdr *ehdr) {
    /* Find last PT_LOAD program header entry */
    Elf64_Phdr *phdr = (Elf64_Phdr *) ELF_OFF(ehdr, ehdr->e_phoff);
    Elf64_Phdr *load_phdr = phdr;
    size_t phdr_count = ehdr->e_phnum;

    for (; phdr_count--; phdr++) {
        if (phdr->p_type == PT_LOAD)
            load_phdr = phdr;
    }

    return load_phdr->p_vaddr + load_phdr->p_memsz;
}

unsigned long elf_get_heap_offset(const Elf64_Ehdr *ehdr) {
    Elf64_Phdr *phdr = (Elf64_Phdr *) ELF_OFF(ehdr, ehdr->e_phoff);
    size_t phdr_count = ehdr->e_phnum;

    for (; phdr_count--; phdr++) {
        if (phdr->p_type != PT_LOAD)
            continue;

        /* Check if the current header's aligned address is
         * the same as the next one's */
        unsigned long alignment = phdr->p_align;
        unsigned long address = phdr->p_vaddr + phdr->p_memsz;
        unsigned long aligned_address = (address + alignment - 1) / alignment * alignment;

        if ((phdr + 1)->p_vaddr != aligned_address)
            return address;
    }

    return 0;
}

unsigned long elf_get_stack_offset(const Elf64_Ehdr *ehdr) {
    Elf64_Phdr *phdr = (Elf64_Phdr *) ELF_OFF(ehdr, ehdr->e_phoff);
    size_t phdr_count = ehdr->e_phnum;

    for (; phdr_count--; phdr++) {
        if (phdr->p_type != PT_LOAD)
            continue;

        /* Check if the current header's aligned address is
         * the same as the next one's */
        unsigned long alignment = phdr->p_align;
        unsigned long address = phdr->p_vaddr + phdr->p_memsz;
        unsigned long aligned_address = (address + alignment - 1) / alignment * alignment;

        if ((phdr + 1)->p_vaddr != aligned_address)
            return (phdr + 1)->p_vaddr;
    }

    return 0;
}
