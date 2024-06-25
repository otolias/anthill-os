#include "string.h"

#include <stddef.h>

char *strrchr(const char *s, int c) {
    const char *ch = s;
    char *pos = NULL;

    while (*ch) {
        if (*ch == c)
            pos = (char *) ch;

        ch++;
    }

    return pos;
}
