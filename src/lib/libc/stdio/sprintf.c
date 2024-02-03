#include "stdio.h"

#include <stdarg.h>

int sprintf(char *str, char *format, ...) {
    va_list args;
    int res;

    va_start(args, format);
    res = vsprintf(str, format, args);
    va_end(args);

    return res;
}
