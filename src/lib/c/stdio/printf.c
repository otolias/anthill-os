#include "stdio.h"

#include <stdarg.h>

int printf(const char *restrict format, ...) {
    va_list args;
    int res;

    va_start(args, format);
    res = vprintf(format, args);
    va_end(args);

    return res;
}
