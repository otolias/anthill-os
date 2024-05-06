#ifndef _STRING_H
#define _STRING_H

#include <sys/types.h>

/*
* Copy _n_ bytes from the object pointed to by _s2_ to the object
* pointed to by _s1_
*
* Returns _s1_
*/
void *memcpy(void *restrict s1, const void *restrict s2, size_t n);

/*
* Copy _n_ bytes from the object pointed to by _s2_ into
* the object pointed to by _s1_. The objects may have
* overlapping areas
*
* Returns _s1_
*/
void *memmove(void *s1, const void *s2, size_t n);

/*
* Copy _c_ as unsigned char into each of the first _n_ bytes
* of the object pointed to by _s_
*
* Returns _s_
*/
 void *memset(void *s, int c, size_t n);

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

/*
* Compute the number of bytes of the initial segment of the
* string pointed to by _s1_ which does not consist of bytes
* from the string pointed to by _s2_
*
* Returns the computed length
*/
size_t strcspn(const char *s1, const char *s2);

/*
* Compute the number of bytes of the initial segment of the
* string pointed to by _s1_ which consists entirely of bytes
* from the string pointed to by _s2_
*
* Returns the computed length
*/
size_t strspn(const char *s1, const char *s2);

/*
* Split the string pointed to by _s_ to a sequence of tokens,
* delimited by a byte from the string pointed to by _sep_
*
* The first call splits _s_, and for each subsequent call
* on the same string, _s_ must be a null pointer
*
* Returns the first byte of a token, or a null pointer if
* there are no tokens left
*/
char *strtok(char *restrict s, const char *restrict sep);

#endif /* _STRING_H */
