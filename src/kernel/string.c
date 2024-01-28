#include <kernel/string.h>

#include <stddef.h>

void* memcpy(void *dest, const void *src, size_t n) {
    const unsigned char *s = src;
    unsigned char *d = dest;
    while (n--)
        *d++ = *s++;
    return dest;
}

void* memset(void *s, int c, size_t n) {
    unsigned char *mem = s;
    while (n--)
        *mem++ = c;
    return s;
}

int strncmp(const char *s1, const char *s2, size_t n) {
    const char *left = s1;
    const char *right = s2;
    while (--n && *left && *right && *left == *right) {
        left++;
        right++;
    }
    return *left - *right;
}

size_t strlen(const char *s) {
    const char *c = s;
    size_t size = 0;
    while (c++) size++;
    return size;
}

size_t strhash(const char *s) {
    size_t fnv_prime = 0x00000100000001B3;
    size_t hash = 0xcbf29ce484222325;
    const char *c = s;

    for(; *c; c++) {
        hash ^= *c;
        hash *= fnv_prime;
    }

    return hash;
}
