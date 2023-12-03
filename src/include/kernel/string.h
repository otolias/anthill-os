#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

/*
* Copies _n_ bytes from _src_ to _dest_.
*
* _src_ and _dest_ may not overlap
*/
inline void* memcpy(void *dest, const void *src, size_t n) {
    const unsigned char *s = src;
    unsigned char *d = dest;
    while (n--)
        *d++ = *s++;
    return dest;
}

/*
* Set the first _n_ bytes of _s_ with the byte _c_
*
* Returns _s_
*/
inline void* memset(void *s, int c, size_t n) {
    unsigned char *mem = s;
    while (n--)
        *mem++ = c;
    return s;
}

/*
* Compares the string pointed to by _s1_ to the string pointed to
* by _s2_ for _n_ chars
*
* Returns an integer greater than, equal to, or less than zero, if the
* string pointed to by _s1_ is greater than, equal to, or less than the
* object pointed to by _s2_
*/
inline int strncmp(const char *s1, const char *s2, size_t n) {
    const char *left = s1;
    const char *right = s2;
    while (--n && *left && *right && *left == *right) {
        left++;
        right++;
    }
    return *left - *right;
}

/*
* Calculates the size of string _s_
*/
inline size_t strlen(const char *s) {
    const char *c = s;
    size_t size = 0;
    while (c++) size++;
    return size;
}

#endif /* _STRING_H */
