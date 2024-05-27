#include "string.h"

void *memcpy(void *restrict s1, const void *restrict s2, size_t n) {
    char *d = s1;
    const char *s = s2;

    size_t i = (n + sizeof(long) - 1) / sizeof(long);
    switch (n % sizeof(long)) {
        case 0: *d++ = *s++; [[fallthrough]];
        case 7: *d++ = *s++; [[fallthrough]];
        case 6: *d++ = *s++; [[fallthrough]];
        case 5: *d++ = *s++; [[fallthrough]];
        case 4: *d++ = *s++; [[fallthrough]];
        case 3: *d++ = *s++; [[fallthrough]];
        case 2: *d++ = *s++; [[fallthrough]];
        case 1: *d++ = *s++;
    }

    while (--i) {
        *(long *) d = *(long *) s;
        d += sizeof(long); s += sizeof(long);
    }

    return s1;
}
