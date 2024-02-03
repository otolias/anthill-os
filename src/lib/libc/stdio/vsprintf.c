#include "stdio.h"

#include "internal/stdio.h"

int vsprintf(char *restrict s, const char *restrict format, va_list ap) {
    return _formatter(s, format, ap);
}
