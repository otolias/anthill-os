#include <kernel/kprintf.h>

#include <stdarg.h>

#include <drivers/uart.h>

#define KPRINTF_BUFFER_SIZE 512

/*
* Puts _val_ with _base_ in location _loc_ of _buffer_
*
* Returns new location after application
*/

static int _kprintf_parse_int(int val, int base, char *buffer, int loc) {
    int div = 1;

    while (val / div >= base) {
        div *= base;
    }

    if (base == 16) {
        buffer[loc++] = '0';
        buffer[loc++] = 'x';
    }

    while (div != 0) {
        int digit = val/ div;
        val %= div;
        div /= base;
        buffer[loc++] = (char) (digit >= 10) ? digit + 55 : digit + 48;
    }

    return loc;
}

/*
* Puts _string_ in _loc_ of _buffer_
*
* Returns new location after application
*/
static int _kprintf_parse_string(char *string, char *buffer, int loc) {
    char c;

    while ((c = *(string++))) {
        buffer[loc++] = c;
    }

    return loc;
}

void kprintf(char *format, ...) {
    va_list args;
    char buffer[KPRINTF_BUFFER_SIZE];
    char c;
    int size = 0;

    va_start(args, format);

    while ((c = *(format++))) {
        if (c != '%') {
            buffer[size++] = c;
            continue;
        }

        c = *(format++);

        switch (c) {
            case 'd':
                size = _kprintf_parse_int(va_arg(args, int), 10, buffer, size);
                break;
            case 'x':
                size = _kprintf_parse_int(va_arg(args, int), 16, buffer, size);
                break;
            case 's':
                size = _kprintf_parse_string(va_arg(args, char*), buffer, size);
                break;
            default:
                break;
        }
    }

    for (int i = 0; i < size; i++) {
        uart_send_char(buffer[i]);
    }

    va_end(args);
}
