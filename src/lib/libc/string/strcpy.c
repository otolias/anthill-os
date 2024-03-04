#include "string.h"

char *strcpy(char *restrict s1, const char *restrict s2) {
    const char *src = s2;
    const char *dest = s1;

    for (; src ; src++, dest++)
        dest = src;

    return s1;
}
