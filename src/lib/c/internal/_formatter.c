#include "internal/stdio.h"

#include <stdbool.h>

static int _char(unsigned char c, unsigned char *buffer, int loc, const int n) {
    if (loc < n)
        buffer[loc] = c;

    return loc + 1;
}

static int _string(const char *string, unsigned char* buffer, int loc, const int n) {
    unsigned char c;

    while ((c = *(string++))) {
        loc = _char(c, buffer, loc, n);
    }

    return loc;
}

static int _uint(unsigned long val, const unsigned char base, unsigned char *buffer,
                 int loc, const int n) {
    unsigned long div = 1;

    while (val / div >= base) {
        div *= base;
    }

    while (div != 0) {
        long digit = val / div;
        val %= div;
        div /= base;
        unsigned char c = (unsigned char) (digit >= 10) ? digit + 87 : digit + 48;
        loc = _char(c, buffer, loc, n);
    }

    return loc;
}

static int _int(long val, const unsigned char base, unsigned char *buffer, int loc,
                const int n) {
    if (val < 0) {
        val *= -1;

        if (base == 10)
            loc = _char('-', buffer, loc, n);
    }

    return _uint(val, base, buffer, loc, n);

}

int formatter(unsigned char* buffer, const char *format, va_list args, size_t max_size) {
    char c;
    int size = 0;
    const int n = (int) max_size;
    bool is_long;

    while ((c = *(format++))) {
        if (c != '%') {
            size = _char(c, buffer, size, n);
            continue;
        }

        c = *(format++);

        if (c == '%') {
            size = _char(c, buffer, size, n);
        } else if (c == 'l') {
            is_long = true;
            c = *(format++);
        } else {
            is_long = false;
        }

        switch (c) {
            case 'c':
                size = _char(va_arg(args, int), buffer, size, n);
                break;
            case 's':
                size = _string(va_arg(args, char*), buffer, size, n);
                break;
            case 'i':
            case 'd':
                size = is_long ?
                    _int(va_arg(args, long), 10, buffer, size, n) :
                    _int(va_arg(args, int), 10,  buffer, size, n);
                break;
            case 'u':
                size = is_long ?
                    _uint(va_arg(args, unsigned long), 10, buffer, size, n) :
                    _uint(va_arg(args, unsigned), 10, buffer, size, n);
                break;
            case 'x':
                size = is_long ?
                    _uint(va_arg(args, unsigned long), 16,  buffer, size, n) :
                    _uint(va_arg(args, unsigned), 16, buffer, size, n);
                break;
            default:
                break;
        }
    }

    return size;
}
