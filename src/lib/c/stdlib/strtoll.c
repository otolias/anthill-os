#include "stdlib.h"

#include <limits.h>

#include "internal/stdlib.h"

long long strtoll(const char *restrict nptr, char **restrict endptr, int base) {
    return (long long) strtox(nptr, endptr, base, LLONG_MAX);
}
