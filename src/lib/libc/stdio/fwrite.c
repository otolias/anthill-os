#include "stdio.h"

size_t fwrite(const void *restrict ptr, size_t size, size_t nitems,
              FILE *restrict stream) {
    if (size == 0 || nitems == 0)
        return 0;

    size_t i;

    for (i = 0; i < nitems; i++) {
        const unsigned char *s = (unsigned char *) ptr + (i * size);
        for (size_t j = 0; j < size; j++) {
            fputc(s[j], stream);
        }
    }

    return i;
}
