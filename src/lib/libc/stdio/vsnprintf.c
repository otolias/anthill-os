#include "stdio.h"

#include <sys/types.h>
#include "internal/stdio.h"

int vsnprintf(char *restrict s, size_t n, const char *restrict format, va_list ap) {
    int written = formatter(s, format, ap, n);
    s[written] = 0;
    return written;
}
