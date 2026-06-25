#include "string.h"

int strcmp(const char *s1, const char *s2) {
    const char *l = s1, *r = s2;
    while (*l && *r && *l == *r) {
        l++; r++;
    }
    return *l - *r;
}
