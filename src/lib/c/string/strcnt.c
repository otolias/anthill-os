#include "string.h"

size_t strcnt(const char *s, const char c) {
    size_t n = 0;

    while (*s)
        if (c == *s++)
            n++;

    return n;
}
