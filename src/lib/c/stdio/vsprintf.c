#include "stdio.h"

#include <limits.h>

int vsprintf(char *restrict s, const char *restrict format, va_list ap) {
    return vsnprintf(s, INT_MAX, format, ap);
}
