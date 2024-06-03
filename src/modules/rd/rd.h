#ifndef _MOD_RD_H
#define _MOD_RD_H

#include <pstring.h>

struct header {
    char name[100];   /* File name */
    char mode[8];     /* File mode */
    char uid[8];      /* User ID */
    char gid[8];      /* Group ID */
    char size[12];    /* Octal representation of file's size in bytes */
    char mtime[12];   /* Latest modification time */
    char checksum[8]; /* File and header Checksum */
    char type;        /* File type flag */
    char link[100];   /* Name of linked file */
    char magic[6];    /* Format representation ("ustar") */
    char version[3];  /* Version representation */
    char uname[32];   /* User Name */
    char gname[32];   /* Group Name */
    char devmajor[8]; /* Major device representation */
    char devminor[8]; /* Minor device representation */
    char prefix[155]; /* Prefix */
    char padding[11]; /* Unused null values */
};

/* Type flags */
#define RDT_FILE  0 /* Normal File */
#define RDT_HLINK 1 /* Hard Link */
#define RDT_SLINK 2 /* Symbolic link */
#define RDT_CHDEV 3 /* Character device */
#define RDT_BLCK  4 /* Block device */
#define RDT_DIR   5 /* Directory */
#define RDT_FIFO  6 /* Named pipe (FIFO) */

/*
* Get header for file _path_
*
* On success, returns a pointer to the file's tar header.
* On failure, returns a null pointer.
*/
struct header* rd_find(const pstring *path);

/*
* Get size of file represented by _header_
*/
size_t rd_get_size(const struct header *header);

/*
* Convert tar type representation to 9p type representation
*/
unsigned int rd_get_type(const struct header *header);

#endif /* _MOD_RD_H */
