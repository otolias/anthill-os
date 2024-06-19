#ifndef _ELF_H
#define _ELF_H

#include <stddef.h>
#include <stdint.h>

#define Elf64_Addr   uint64_t
#define Elf64_Off    uint64_t
#define Elf64_Half   uint16_t
#define Elf64_Word   uint32_t
#define Elf64_Sword  int32_t
#define Elf64_Xword  uint64_t
#define Elf64_Sxword int64_t

#define ELF_OFF(base, offset) (((unsigned long) base) + (offset))

/* Elf header */
typedef struct {
    unsigned char e_ident[16]; /* ELF Identification */
    Elf64_Half    e_type;      /* Object File Type */
    Elf64_Half    e_machine;   /* Machine type */
    Elf64_Word    e_version;   /* ELF Version */
    Elf64_Addr    e_entry;     /* Program entry point */
    Elf64_Off     e_phoff;     /* Program header offset */
    Elf64_Off     e_shoff;     /* Section header offset */
    Elf64_Word    e_flags;     /* Processor-specific flags */
    Elf64_Half    e_ehsize;    /* Header size */
    Elf64_Half    e_phentsize; /* Program header size */
    Elf64_Half    e_phnum;     /* Number of entries in program header */
    Elf64_Half    e_shentsize; /* Size of a section header table */
    Elf64_Half    e_shnum;     /* Number of entries in section header */
    Elf64_Half    e_shstrndx;  /* Index of section names. */
} Elf64_Ehdr;

/* ELF header identification offsets */
#define EI_CLASS  4  /* File class */

typedef struct {
    Elf64_Word  p_type;   /* Type of segment */
    Elf64_Word  p_flags;  /* Segment attributes */
    Elf64_Off   p_offset; /* Offset in file */
    Elf64_Addr  p_vaddr;  /* Virtual address in memory */
    Elf64_Addr  p_paddr;  /* Reserved */
    Elf64_Xword p_filesz; /* Size of segment in file */
    Elf64_Xword p_memsz;  /* Size of segment in memory */
    Elf64_Xword p_align;  /* Alignment of segment */
} Elf64_Phdr;

/* Program header type values */
#define PT_LOAD    1
#define PT_DYNAMIC 2

/* Dynamic table entry */
typedef struct {
    Elf64_Sxword d_tag;
    union {
        Elf64_Xword d_val;
        Elf64_Addr d_ptr;
    };
} Elf64_Dyn;

/* Dynamic table entry tag values */
#define DT_NULL          0  /* End of array */
#define DT_NEEDED        1  /* Needed library */
#define DT_PLTRELSZ      2  /* PLT relocation entries total size */
#define DT_PLTGOT        3  /* PLT and GOT table address */
#define DT_STRTAB        5  /* String table address */
#define DT_SYMTAB        6  /* Symbol table address */
#define DT_RELA          7  /* Relocation table address (with addend) */
#define DT_RELASZ        8  /* Relocation table total size */
#define DT_RELAENT       9  /* Relocation table entry size */
#define DT_REL           17 /* Relocation table address (without addend) */
#define DT_PLTREL        20 /* PLT relocation entries type */
#define DT_JMPREL        23 /* PLT relocation entries address */
#define DT_INIT_ARRAY    25 /* Initialisation functions array pointer */
#define DT_FINI_ARRAY    26 /* Deinitialisation functions array pointer */
#define DT_INIT_ARRAY_SZ 27 /* Initialisation functions array size (in bytes) */
#define DT_FINI_ARRAY_SZ 28 /* Deinitialisation functions array size (in bytes) */

/* Symbol table entry */
typedef struct {
    Elf64_Word st_name;     /* Symbol name index */
    unsigned char st_info;  /* Type and binding attributes */
    unsigned char st_other; /* Reserved */
    Elf64_Half st_shndx;    /* Section table index */
    Elf64_Addr st_value;    /* Symbol value */
    Elf64_Xword st_size;    /* Size of object */
} Elf64_Sym;

/* Relocation entry (with addend) */
typedef struct {
    Elf64_Addr r_offset;   /* Address of reference */
    Elf64_Xword r_info;    /* Symbol index and type of relocation */
    Elf64_Sxword r_addend; /* Constant addend */
} Elf64_Rela;

/* Relocation entry (without addend) */
typedef struct {
    Elf64_Addr r_offset; /* Address of reference */
    Elf64_Xword r_info;  /* Symbol index and type of relocation */
} Elf64_Rel;

#define ELF64_R_SYM(i) ((i) >> 32)
#define ELF64_R_TYPE(i) ((i) & 0xffffffffL)

/* Dynamic relocations */
#define R_AARCH64_GLOB_DAT   1025
#define R_AARCH64_JUMP_SLOT  1026
#define R_AARCH64_RELATIVE   1027

/*
* Validate elf header
*
* Returns 0 if valid header, -1 if not
*/
short elf_validate(const Elf64_Ehdr *ehdr);

/*
* Calculate elf image size
*/
size_t elf_get_image_size(const Elf64_Ehdr *ehdr);

#endif /* _ELF_H */
