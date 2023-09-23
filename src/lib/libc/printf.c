#include <stdio.h>

#include <kernel/syscalls.h>

#define PRINTF_BUFFER_SIZE  1024

/*
* Puts _val_ with _base_ in location _loc_ of _buffer_
*
* Returns new location after application
*/
static int _parse_int(long int val, const int base, char* buffer, int loc) {
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

/*
* Puts _c_ in _loc_ of _buffer_
*/
static int _parse_char(unsigned char c, char *buffer, int loc) {
    buffer[loc++] = c;
    return loc;
}

/*
* Puts _string_ in _loc_ of _buffer_
*
* Returns new location after application
*/
static int _parse_string(char *string, char* buffer, int loc) {
    unsigned char c;

    while ((c = *(string++))) {
        loc = _parse_char(c, buffer, loc);
    }

    return loc;
}

/*
* Formats _format_ according to _args_ and puts the result in _buffer_
*/
static int _formatter(char* buffer, char *format, va_list args) {
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

    buffer[size] = '\x00';
    return size;
}

int vsprintf(char *str, char *format, va_list args) {
    return _formatter(str, format, args);
}

int sprintf(char *str, char *format, ...) {
    va_list args;
    int res;

    va_start(args, format);
    res = vsprintf(str, format, args);
    va_end(args);

    return res;
}

int printf(char *format, ...) {
    va_list args;
    char buffer[PRINTF_BUFFER_SIZE];
    /* int res; */

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    /* res = SYSCALL_1(SYS_WRITE, (long) buffer); */

    /* return res; */
}
