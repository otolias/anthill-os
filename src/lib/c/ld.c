#include <stddef.h>

#include <kernel/elf.h>

struct dynamic_object {
    size_t base;          /* Base address of object */
    size_t pltrelsz;      /* PLT relocation table total size */
    size_t symtab;        /* Symbol table address */
    size_t rela;          /* Relocation table with addend address */
    size_t relasz;        /* Relocation table total size */
    size_t relaent;       /* Relocation table entry size */
    size_t jmprel;        /* PLT relocation table address */
};

#define SP_FILE_OFFSET 0
#define SP_ADDR_OFFSET 1

#define JMP(pc, sp) __asm__ ( \
    "mov sp, %1 \n" \
    "br %0; \n" \
    : : "r"(pc), "r"(sp) : "memory" \
    )

/* Entry point */
__asm__(
    ".section .text \n"
    ".global _start \n"
    "_start: \n"
    "mov x0, sp \n"
    "b linker_init\n"
);

/* Parse and initialise _dyn_obj_ for dynamic section _dyn_entry_*/
static inline void _parse_dyn_obj(struct dynamic_object *dyn_obj, const Elf64_Dyn *dyn_entry) {
    for (; dyn_entry->d_tag != DT_NULL; dyn_entry++) {
        switch (dyn_entry->d_tag) {
            case DT_PLTRELSZ:
                dyn_obj->pltrelsz = dyn_entry->d_val;
                break;
            case DT_SYMTAB:
                dyn_obj->symtab = dyn_entry->d_ptr;
                break;
            case DT_RELA:
                dyn_obj->rela = dyn_entry->d_ptr;
                break;
            case DT_RELASZ:
                dyn_obj->relasz = dyn_entry->d_val;
                break;
            case DT_RELAENT:
                dyn_obj->relaent = dyn_entry->d_val;
                break;
            case DT_JMPREL:
                dyn_obj->jmprel = dyn_entry->d_ptr;
                break;
        }
    }
}

/*
* Perform relative relocations with addend
*/
static void linker_rela_relocate(const Elf64_Rela *reloc_entry, const struct dynamic_object *dyn_obj) {
    size_t *offset = (size_t *) (dyn_obj->base + reloc_entry->r_offset);
    const Elf64_Sym *symbol = (Elf64_Sym *) ELF_OFF(dyn_obj->base, dyn_obj->symtab) +
        ELF64_R_SYM(reloc_entry->r_info);

    switch (ELF64_R_TYPE(reloc_entry->r_info)) {
        case R_AARCH64_GLOB_DAT:
        case R_AARCH64_JUMP_SLOT:
            *offset = dyn_obj->base + symbol->st_value + reloc_entry->r_addend;
            break;
        case R_AARCH64_RELATIVE:
            *offset = dyn_obj->base + reloc_entry->r_addend;
            break;
    }
}

__attribute__((__visibility__("hidden")))
void linker_init(size_t *sp) {
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *) *(sp + SP_FILE_OFFSET);
    size_t process_address = *(sp + SP_ADDR_OFFSET);
    Elf64_Phdr *phdr_entry = (Elf64_Phdr *) ELF_OFF(ehdr, ehdr->e_phoff);
    Elf64_Dyn *dyn_entry = NULL;

    /* Find dynamic section */
    for (size_t i = 0; i < ehdr->e_phnum; i++) {
        if (phdr_entry->p_type == PT_DYNAMIC) {
            dyn_entry = (Elf64_Dyn *) ELF_OFF(ehdr, phdr_entry->p_offset);
            break;
        }
        phdr_entry++;
    }

    /* Create dynamic object */
    struct dynamic_object dyn_obj;
    dyn_obj.base = process_address;
    _parse_dyn_obj(&dyn_obj, dyn_entry);

    /* Perform dynamic relocations */
    Elf64_Rela *reloc_entry;
    size_t reloc_count;

    reloc_entry = (Elf64_Rela *) ELF_OFF(ehdr, dyn_obj.rela);
    reloc_count = dyn_obj.relasz / dyn_obj.relaent;
    while(reloc_count--) {
        linker_rela_relocate(reloc_entry++, &dyn_obj);
    }

    reloc_entry = (Elf64_Rela *) ELF_OFF(ehdr, dyn_obj.jmprel);
    reloc_count = dyn_obj.pltrelsz / sizeof(Elf64_Rela);
    while(reloc_count--) {
        linker_rela_relocate(reloc_entry++, &dyn_obj);
    }

    JMP(ELF_OFF(dyn_obj.base, ehdr->e_entry), sp);
}
