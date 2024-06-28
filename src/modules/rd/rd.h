#ifndef _MOD_RD_H
#define _MOD_RD_H

#include <fcall.h>
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
    char version[2];  /* Version representation */
    char uname[32];   /* User Name */
    char gname[32];   /* Group Name */
    char devmajor[8]; /* Major device representation */
    char devminor[8]; /* Minor device representation */
    char prefix[155]; /* Prefix */
    char padding[12]; /* Unused null values */
};

/* Type flags */
#define RDT_FILE  '0' /* Normal File */
#define RDT_HLINK '1' /* Hard Link */
#define RDT_SLINK '2' /* Symbolic link */
#define RDT_CHDEV '3' /* Character device */
#define RDT_BLCK  '4' /* Block device */
#define RDT_DIR   '5' /* Directory */
#define RDT_FIFO  '6' /* Named pipe (FIFO) */

/*
* Get header for file _path_
*
* On success, returns a pointer to the file's tar header.
* On failure, returns a null pointer.
*/
struct header* rd_find(const pstring *path);

/*
* Puts pstring representation of _header_ filename in _buf_, with a limit of _n_ bytes.
*
* On success, returns the number of bytes put.
* On failure, returns 0 and doesn't modify the buffer.
*/
unsigned rd_get_name(const struct header *header, unsigned char *buf, size_t n);

/*
* Get size of file represented by _header_
*/
size_t rd_get_size(const struct header *header);

/*
* Put up to _n_ bytes of stat information for file represented by _header_ to _stat_.
*
* On success, returns total number of bytes to be written.
* On failure, returns 0.
*/
unsigned rd_get_stat(const struct header *header, struct fcall_stat *stat, size_t n);

/*
* Convert tar type representation to 9p type representation
*/
unsigned int rd_get_type(const struct header *header);

/*
* Put up to _n_ bytes of _count_ directory entries represented by _header_ to
* _buf_ starting from _offset_.
*
* Returns the number of bytes to be written.
*/
unsigned rd_read_dir(const struct header *header, unsigned char *buf, size_t offset,
                      unsigned count, size_t n);

/*
* Put _count_ bytes of file represented by _header_ to _buf_ starting from _offset_.
*
* Returns the number of bytes read.
*/
unsigned rd_read_file(const struct header *header, unsigned char *buf, size_t offset,
                      unsigned count);

#endif /* _MOD_RD_H */
