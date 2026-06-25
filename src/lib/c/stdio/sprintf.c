#include "stdio.h"

#include <stdarg.h>

int sprintf(char *restrict s, const char *restrict format, ...) {
    va_list args;
    int res;

    va_start(args, format);
    res = vsprintf(s, format, args);
    va_end(args);

    return res;
}
