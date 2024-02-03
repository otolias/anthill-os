#include "internal/stdio.h"

int _formatter(char* buffer, const char *format, va_list args) {
    char c;
    int size = 0;

    while ((c = *(format++))) {
        if (c != '%') {
            buffer[size++] = c;
            continue;
        }

        c = *(format++);

        switch (c) {
            case 'c':
                size = _parse_char(va_arg(args, unsigned int), buffer, size);
                break;
            case 'd':
                size = _parse_int(va_arg(args, int), 10,  buffer, size);
                break;
            case 's':
                size = _parse_string(va_arg(args, char*), buffer, size);
                break;
            case 'u':
                size = _parse_int(va_arg(args, unsigned long int), 10, buffer, size);
                break;
            case 'x':
                size = _parse_int(va_arg(args, long int), 16,  buffer, size);
                break;
            default:
                break;
        }
    }

    buffer[size] = 0;
    return size;
}

int _parse_char(unsigned char c, char *buffer, int loc) {
    buffer[loc++] = c;
    return loc;
}

int _parse_int(long int val, const int base, char* buffer, int loc) {
    int div = 1;

    if (val < 0) {
        val *= -1;

        if (base == 10)
            buffer[loc++] = '-';
    }

    while (val / div >= base) {
        div *= base;
    }

    while (div != 0) {
        int digit = val / div;
        val %= div;
        div /= base;
        buffer[loc++] = (char) (digit >= 10) ? digit + 87 : digit + 48;
    }

    return loc;
}

int _parse_string(char *string, char* buffer, int loc) {
    unsigned char c;

    while ((c = *(string++))) {
        loc = _parse_char(c, buffer, loc);
    }

    return loc;
}
