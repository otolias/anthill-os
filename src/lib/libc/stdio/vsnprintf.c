#include "stdio.h"

#include <sys/types.h>
#include "internal/stdio.h"

int vsnprintf(char *restrict s, size_t n, const char *restrict format, va_list ap) {
    return formatter(s, format, ap, n);
}
