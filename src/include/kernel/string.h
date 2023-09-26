#ifndef _STRING_H
#define _STRING_H

#include <stddef.h>

/*
* Compares the string pointed to by _s1_ to the string pointed to
* by _s2_ for _n_ chars
*
* Returns an integer greater than, equal to, or less than zero, if the
* string pointed to by _s1_ is greater than, equal to, or less than the
* object pointed to by _s2_
*/
extern inline int strncmp(const char *s1, const char *s2, size_t n) {
    const char *left = s1;
    const char *right = s2;
    while (n-- && *left && *right && *left == *right) {
        left++;
        right++;
    }
    return *left - *right;
}

#endif /* _STRING_H */
