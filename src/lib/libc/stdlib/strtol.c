#include "stdlib.h"

#include <limits.h>

#include "internal/stdlib.h"

long strtol(const char *restrict nptr, char **restrict endptr, int base) {
    return (long) strtox(nptr, endptr, base, LONG_MAX);
}
