#ifndef _STRING_H
#define _STRING_H

#include <sys/types.h>

/*
* Locate the first occurence of _c_ in the string
* pointed to by _s_
*
* On success, returns a pointer to the byte.
* On failure, returns null pointer
*/
char *strchr(const char *s, int c);

/*
* Compare the string pointed to by _s1_ to the
* string pointed to by _s2_
*
* Returns an integer greater than, equal to, or
* less than 0 if the string pointed to by _s1_ is
* greater than, equal to, or less than the string
* pointed to by _s2_.
*/
int strcmp(const char *s1, const char *s2);

/*
* Copy the string pointed to by _s2_ to the string
* pointed to by _s1_
*
* Returns _s1_
*/
char *strcpy(char *restrict s1, const char *restrict s2);

/*
* Calculate the number of bytes in the string to which
* _s_ points, not including the terminating characted.
*/
size_t strlen(const char* s);

/*
* Copy up to _n_ bytes from the string pointed to
* by _s2_ to the string pointed to by _s1_
*
* Returns _s1_
*/
char *strncpy(char *restrict s1, const char *restrict s2, size_t n);

#endif /* _STRING_H */
