#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/types.h>

/*
* Allocate unused space for an object of _size_ in bytes
*
* On success, returns a pointer to the allocated space.
* On failure, retrus a null pointer
*/
void* malloc(size_t size);

/*
* Deallocate space pointed to by _ptr_
*/
void free(void *ptr);

/*
* Terminate process
*
* Status is currently unsupported
*/
void exit(int status);

#endif /* _STDLIB_H */
