#ifndef _KERNEL_STRING_H
#define _KERNEL_STRING_H

#include <stddef.h>

/*
* Copies _n_ bytes from _src_ to _dest_.
*
* _src_ and _dest_ may not overlap
*/
void* memcpy(void *dest, const void *src, size_t n);

/*
* Set the first _n_ bytes of _s_ with the byte _c_
*
* Returns _s_
*/
void* memset(void *s, int c, size_t n);

/*
* Compares the string pointed to by _s1_ to the string pointed to
* by _s2_ for _n_ chars
*
* Returns an integer greater than, equal to, or less than zero, if the
* string pointed to by _s1_ is greater than, equal to, or less than the
* object pointed to by _s2_
*/
int strncmp(const char *s1, const char *s2, size_t n);

/*
* Calculates the size of string _s_
*/
size_t strlen(const char *s);

/*
* FNV-1a string hashing
*/
size_t strhash(const char *s);

#endif /* _KERNEL_STRING_H */
