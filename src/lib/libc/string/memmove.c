#include "string.h"

void *memmove(void *s1, const void *s2, size_t n) {
    const char *s = s2;
    char *d = s1;

    if (s == d) return d;

    if (s > d)
        for (; n; n--)
            *d++ = *s++;
    else
        for (; n; n--)
            d[n] = s[n];

    return d;
}
