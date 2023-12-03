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

unsigned long elf_get_stack_offset(const Elf64_Ehdr *ehdr) {
    Elf64_Phdr *phdr = (Elf64_Phdr *) ELF_OFF(ehdr, ehdr->e_phoff);
    size_t phdr_count = ehdr->e_phnum;
    Elf64_Dyn *dyn = 0;

    for (; phdr_count--; phdr++) {
        if (phdr->p_type == PT_DYNAMIC) {
            dyn = (Elf64_Dyn *) (ELF_OFF(ehdr, phdr->p_offset));
            break;
        }
    }

    if (!dyn)
        return 0;

    for (; dyn->d_tag != DT_NULL; dyn++) {
        if (dyn->d_tag == DT_INIT_ARRAY)
            return dyn->d_ptr;
    }

    return 0;
}
