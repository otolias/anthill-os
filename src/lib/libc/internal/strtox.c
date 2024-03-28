#include "internal/stdlib.h"

#include <ctype.h>
#include <errno.h>

unsigned long long strtox(const char *restrict str, char **restrict endptr, int base,
                          unsigned long long limit) {
    const char *c = str;
    unsigned long long res = 0, tmp;
    int multiplier = 1;

    if (!(base == 0 || base == 10)) {
        errno = EINVAL;
        return 0;
    }

    while(isspace(*c)) c++;

    if (*c == '-') {
        multiplier = -1;
        c++;
    } else if (*c == '+')
        c++;

    if (!isdigit(*c)) {
        errno = EINVAL;
        return 0;
    }

    do {
        tmp = res;
        res *= 10;
        res += *c - '0';

        /* Check for wraparound */
        if (res < tmp) {
            errno = ERANGE;
            return 0;
        }
    } while(isdigit(*(++c)));

    if (res > limit) {
        errno = ERANGE;
        return 0;
    }

    res *= multiplier;

    if (endptr)
        *endptr = (char *) c;

    return res;
}
