#include "string.h"

 int memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *l = s1, *r = s2;

    while (*l == *r && n--) { l++; r++; }

    return n ? *l - *r : 0;
}
