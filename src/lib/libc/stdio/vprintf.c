#include "stdio.h"

#include <stdarg.h>

int vprintf(const char *restrict format, va_list ap) {
    return vfprintf(stdout, format, ap);
}
