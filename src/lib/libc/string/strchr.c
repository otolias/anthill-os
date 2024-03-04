#include "string.h"

#include <stddef.h>

char *strchr(const char *s, int c) {
    const char *ch = s;
    while (ch++) {
        if (*ch == c)
            return (char *) ch;
    }
    return NULL;
}
