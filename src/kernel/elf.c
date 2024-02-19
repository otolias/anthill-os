#include <kernel/elf.h>

#include <kernel/string.h>
#include <kernel/sys/types.h>

short elf_validate(const Elf64_Ehdr *ehdr) {
    if (strncmp((char *) ehdr->e_ident, "\x7f""ELF", 4))
        return 0;

    if (ehdr->e_type != 3)
        return 0;

    if (ehdr->e_machine != 0xb7)
        return 0;

    return 1;
}

size_t elf_get_image_size(const Elf64_Ehdr *ehdr) {
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
