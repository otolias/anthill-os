#include "stdlib.h"

#include <limits.h>

#include "internal/stdlib.h"

unsigned long long strtoull(const char *restrict str, char **restrict endptr, int base) {
    return strtox(str, endptr, base, ULLONG_MAX);
}
