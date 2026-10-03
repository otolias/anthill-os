/*
* Kernel-wide error codes
*/
#ifndef _KERNEL_ERROR_H
#define _KERNEL_ERROR_H

enum kern_err {
    ERR_OK,
    ERR_ELF_INV, /* Invalid ELF file */
    ERR_ELF_MAC, /* ELF file is for a different architecture */
    ERR_ELF_UNS, /* Unsupported ELF file type */
    ERR_MEM_EXS, /* Tried to map already mapped page */
    ERR_MEM_FLG, /* Invalid permission flags */
    ERR_MEM_OOM, /* Out of memory */
    ERR_MEM_UNM, /* Tried to unmap already unmapped page */
    ERR_MSB_CHF, /* Channel array is full */
    ERR_MSB_CHN, /* Channel not found */
    ERR_MSB_EXS, /* Message buffer name already exists */
    ERR_MSB_FND, /* Message buffer not found */
    ERR_MSB_LNG, /* The name given is too long */
    ERR_MSB_SIZ, /* Invalid size argument */
};

#endif /* _KERNEL_ERROR_H */
