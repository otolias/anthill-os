#include "internal/stdio.h"

#include <stdbool.h>

static int _char(char c, char *buffer, int loc) {
    buffer[loc++] = c;
    return loc;
}

static int _string(const char *string, char* buffer, int loc) {
    unsigned char c;

    while ((c = *(string++))) {
        loc = _char(c, buffer, loc);
    }

    return loc;
}

static int _uint(unsigned long val, const unsigned char base, char *buffer, int loc) {
    unsigned long div = 1;

    while (val / div >= base) {
        div *= base;
    }

    while (div != 0) {
        long digit = val / div;
        val %= div;
        div /= base;
        buffer[loc++] = (char) (digit >= 10) ? digit + 87 : digit + 48;
    }

    return loc;
}

static int _int(long val, const unsigned char base, char *buffer, int loc) {
    if (val < 0) {
        val *= -1;

        if (base == 10)
            buffer[loc++] = '-';
    }

    return _uint(val, base, buffer, loc);

}

int formatter(char* buffer, const char *format, va_list args) {
    char c;
    int size = 0;
    bool is_long;

    while ((c = *(format++))) {
        if (c != '%') {
            buffer[size++] = c;
            continue;
        }

        c = *(format++);

        if (c == 'l') {
            is_long = true;
            c = *(format++);
        } else {
            is_long = false;
        }

        switch (c) {
            case 'c':
                size = _char(va_arg(args, int), buffer, size);
                break;
            case 's':
                size = _string(va_arg(args, char*), buffer, size);
                break;
            case 'i':
            case 'd':
                size = is_long ?
                    _int(va_arg(args, long), 10, buffer, size) :
                    _int(va_arg(args, int), 10,  buffer, size);
                break;
            case 'u':
                size = is_long ?
                    _uint(va_arg(args, unsigned long), 10, buffer, size) :
                    _uint(va_arg(args, unsigned), 10, buffer, size);
                break;
            case 'x':
                size = is_long ?
                    _uint(va_arg(args, unsigned long), 16,  buffer, size) :
                    _uint(va_arg(args, unsigned), 16, buffer, size);
                break;
            default:
                break;
        }
    }

    buffer[size] = 0;
    return size;
}
