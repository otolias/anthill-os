#include "stdio.h"

#include <stdarg.h>
#include <sys/types.h>

int snprintf(char *restrict s, size_t n, const char *restrict format, ...) {
    va_list args;
    int res;

    va_start(args, format);
    res = vsnprintf(s, n, format, args);
    va_end(args);

    return res;
}
