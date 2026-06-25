#include "stdlib.h"

#include <limits.h>

#include "internal/stdlib.h"

unsigned long strtoul(const char *restrict str, char **restrict endptr, int base) {
    return (unsigned long) strtox(str, endptr, base, ULONG_MAX);
}
