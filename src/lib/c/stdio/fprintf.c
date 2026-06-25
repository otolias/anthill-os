#include "stdio.h"

#include <stdarg.h>

int fprintf(FILE *restrict stream, const char *restrict format, ...) {
    va_list args;
    int res;

    va_start(args, format);
    res = vfprintf(stream, format, args);
    va_end(args);

    return res;
}
