/*
* tar archive related functions
*/

#include <kernel/kprintf.h>
#include <kernel/string.h>

#include <kernel/tar.h>

typedef struct {
    char name[100];      // File name
    char mode[8];        // File mode
    char uid[8];         // User ID
    char gid[8];         // Group ID
    char size[12];       // File size in bytes (octal)
    char mtime[12];      // Latest modification time
    char chksum[8];      // File and header Checksum
    char typeflag[1];    // File type flag
    char linkname[100];  // Name of linked file
    char magic[6];       // Format representation ("ustar")
    char version[3];     // Version representation
    char uname[32];      // User Name
    char gname[32];      // Group Name
    char devmagor[8];    // Major device representation
    char devminor[8];    // Minor device representation
    char prefix[155];    // Prefix
    char padding[11];    // Unused null values
} tar_header;

/*
* Get file size
*
* Converts file size from octal string represantation to
* long int
*/
static long tar_get_size(tar_header *header) {
    long n = 0;
    int size = sizeof(header->size);
    char *c = header->size;

    while (size-- > 0 && *c) {
        n *= 8;
        n += *c - '0';
        c++;
    }

    return n;
}

void* tar_lookup(const void *archive, const char *filename) {
    tar_header *header = (tar_header *) archive;

    while(!strncmp(header->magic, "ustar", sizeof(header->magic))) {
        if (!strncmp(header->name, filename, sizeof(header->name)))
            return header;

        int blocks = ((tar_get_size(header) + 511) / 512) + 1;

        header += blocks;
    }

    return NULL;
}

