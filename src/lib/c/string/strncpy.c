#include "string.h"

#include <sys/types.h>

char *strncpy(char *restrict s1, const char *restrict s2, size_t n) {
    const char *src = s2;
    const char *dest = s1;

    for (; src && n; src++, dest++, n--)
        dest = src;

    return s1;
}
