#include "kernel/io.h"

#include <kernel/arch/io.h>
#include <stdarg.h>
#include <stdbool.h>

#define IO_BUFFER_SIZE 1024

unsigned char io_buffer[IO_BUFFER_SIZE];

/*
* Put char _c_ in position _pos_ of io_buffer, if the buffer isn't full. Returns
* the number of bytes written.
*/
static int _char(const unsigned char c, int pos) {
    if (pos >= IO_BUFFER_SIZE)
        return 0;

    io_buffer[pos] = c;
    return 1;
}

/*
* Put string _s_ in position _pos_ of io_buffer, if the buffer isn't full. Returns
* the number of bytes written.
*/
static int _string(const unsigned char *s, int pos) {
    int written = 0;
    unsigned char c;

    while ((c = *(s++)))
        written += _char(c, pos + written);

    return written;
}

/*
* Put unsigned long _val_ with _base_ in position _pos_ of io_buffer, if the
* buffer isn't full. Returns the number of bytes written.
*/
static int _uint(unsigned long val, const unsigned base, int pos) {
    int written = 0;
    unsigned long div = 1;

    while (val / div >= base)
        div *= base;

    while (div != 0) {
        unsigned long digit = val / div;
        val %= div;
        div /= base;
        unsigned char c = (unsigned char) (digit >= 10) ? digit + 87 : digit + 48;
        written += _char(c, pos + written);
    }

    return written;
}

static int _int(long val, const unsigned base, int pos) {
    int written = 0;

    if (val < 0) {
        val *= -1;

        if (base == 10)
            written += _char('-', pos + written);
    }

    written += _uint(val, base, pos + written);
    return written;
}

int io_fmt(const char *restrict format, ...) {
    va_list args;
    int size = 0;
    char c;

    va_start(args, format);

    while ((c = *(format++))) {
        if (c != '%') {
            size += _char(c, size);
            continue;
        }

        c = *(format++);
        if (c == '%') {
            size += _char(c, size);
            continue;
        }

        bool is_long = false;

        if (c == 'l') {
            is_long = true;
            c = *(format++);
        }

        switch (c) {
            case 'c':
                size += _char(va_arg(args, int), size);
                break;
            case 'i':
            case 'd':
                size += is_long ?
                    _int(va_arg(args, long), 10, size) :
                    _int(va_arg(args, int), 10, size);
                break;
            case 's':
                size += _string(va_arg(args, unsigned char *), size);
                break;
            case 'u':
                size += is_long ?
                    _uint(va_arg(args, unsigned long), 10, size) :
                    _uint(va_arg(args, unsigned), 10, size);
                break;
            case 'x':
                size += is_long ?
                    _uint(va_arg(args, unsigned long), 16, size) :
                    _uint(va_arg(args, unsigned), 16, size);
                break;
            default:
                break;
        }
    }

    va_end(args);

    return io_write(io_buffer, size);
}
