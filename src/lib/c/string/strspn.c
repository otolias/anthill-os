#include <string.h>

#include <stdbool.h>
#include <sys/types.h>

size_t strspn(const char *s1, const char *s2) {
    const char *c1, *c2;

    for (c1 = s1; *c1; c1++) {
        bool found = false;

        for (c2 = s2; *c2; c2++) {
            if (*c1 == *c2)
                found = true;
        }

        if (!found) break;
    }

    return c1 - s1;
}
