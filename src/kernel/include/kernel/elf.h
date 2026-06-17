#ifndef _KERNEL_ELF_H
#define _KERNEL_ELF_H

#include <kernel/error.h>
#include <stddef.h>
#include <stdint.h>

/* Elf header */
struct elf64_ehdr {
    unsigned char e_ident[16]; /* ELF Identification */
    uint16_t e_type;           /* Object File Type */
    uint16_t e_machine;        /* Machine type */
    uint32_t e_version;        /* ELF Version */
    uint64_t e_entry;          /* Program entry point */
    uint64_t e_phoff;          /* Program header offset */
    uint64_t e_shoff;          /* Section header offset */
    uint32_t e_flags;          /* Processor-specific flags */
    uint16_t e_ehsize;         /* Header size */
    uint16_t e_phentsize;      /* Program header size */
    uint16_t e_phnum;          /* Number of entries in program header */
    uint16_t e_shentsize;      /* Size of a section header table */
    uint16_t e_shnum;          /* Number of entries in section header */
    uint16_t e_shstrndx;       /* Index of section names. */
};

/* Object File Types (e_type) */
#define ET_EXEC 2

struct elf64_phdr {
    uint32_t p_type;   /* Type of segment */
    uint32_t p_flags;  /* Segment attributes */
    uint64_t p_offset; /* Offset in file */
    uint64_t p_vaddr;  /* Virtual address in memory */
    uint64_t p_paddr;  /* Reserved */
    uint64_t p_filesz; /* Size of segment in file */
    uint64_t p_memsz;  /* Size of segment in memory */
    uint64_t p_align;  /* Alignment of segment */
};

/* Segment Types (p_type) */
#define PT_LOAD 1

/* Segment Attributes (p_flags) */
#define PF_X 1 // Execute permission
#define PF_W 2 // Write permission
#define PF_R 4 // Read permission

struct task;

struct elf_r_addr {
    void *addr;
    enum kern_err err;
};

/*
* Parse ELF header _ehdr_ and create process image using the translation table
* at the virtual address pointed to by _tran_table_.
*
* Returns struct elf_r_img:
* - On success, _addr_ is a pointer to the virtual address at the start of the
*   process image (the stack) and _err_ is set to KERN_OK.
* - On failure, _addr_ is NULL and _err_ is set to indicate the error.
*/
struct elf_r_addr elf_create_proc_image(const struct elf64_ehdr *ehdr, void *tran_table);

/*
* Validate ELF header.
*
* Returns enum kern_err.
*/
enum kern_err elf_validate(const struct elf64_ehdr *ehdr);

#endif /* _KERNEL_ELF_H */
