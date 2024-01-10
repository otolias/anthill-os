#ifndef _STDLIB_H
#define _STDLIB_H

#include <stddef.h>

/*
* Allocate space for an object of size _size_
*
* Returns either a null pointer or a pointer to the
* allocated space
*/
void* malloc(size_t size);

/*
* Free space pointed to by _ptr_
*/
void free(void *ptr);

/*
* Terminate process
*/

void exit(void);

#endif /* _STDLIB_H */
