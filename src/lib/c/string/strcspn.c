#include <string.h>

size_t strcspn(const char *s1, const char *s2) {
    const char *c1 = s1, *c2;

    for (; *c1; c1++)
        for (c2 = s2; *c2; c2++)
            if (*c1 == *c2)
                return c1 - s1;

    return c1 - s1;
}
