/*
* The ramdisk is a tar archive loaded at a fixed position
*/

#ifndef _RAMDISK_H
#define _RAMDISK_H

/*
* Return pointer to _filename_ data in ramdisk,
* or null if it doesn't exist
*/
void* ramdisk_lookup(const char *filename);

#endif /* _RAMDISK_H */
