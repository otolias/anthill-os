#ifndef _KERNEL_RD_H
#define _KERNEL_RD_H

/*
* Lookup _filename_ in ramdisk. Returns pointer to data on success, or null if
* not found.
*/
void* rd_lookup(const char *filename);

#endif /* _KERNEL_RD_H */
