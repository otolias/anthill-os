#ifndef _TAR_H
#define _TAR_H

/*
* Return pointer to _filename_ in tar archive _archive_,
* or null if it doesn't exist
*/
void* tar_lookup(const void *archive, const char *filename);

#endif /* _TAR_H */
