#include <stdarg.h>
#include <stdint.h>
#include "debug_printf.h"
#include "uart.h"

/*
* Converts an int to a character array with base and writes it to the given buffer
*/
static void integer_to_string(char* buffer, uint32_t number, uint32_t base) {
    uint32_t div = 1;

    while (number / div >= base) {
        div *= base;
    }

    if (base == 16) {
        *buffer++ = '0';
        *buffer++ = 'x';
    }

    while (div != 0) {
        uint8_t digit = number / div;
        number %= div;
        div /= base;
        *buffer++ = (char) (digit >= 10) ? digit + 55 : digit + 48;
    }
}

void dbg_printf(char *format, ...) {
    va_list va;
    char c;
    char buffer[16];

    va_start(va, format);

    while ((c = *(format++))) {
        if (c != '%') {
            uart_send_char(c);
        } else {
            c = *(format++);

            switch (c) {
                case 'd':
                    integer_to_string(buffer, va_arg(va, uint32_t), 10);
                    uart_puts(buffer);
                    *buffer = 0;
                    break;
                case 'x':
                    integer_to_string(buffer, va_arg(va, uint32_t), 16);
                    uart_puts(buffer);
                    *buffer = 0;
                    break;
                case 's':
                    uart_puts(va_arg(va, char*));
                default:
                    break;
            }
        }
    }
    va_end(va);
}
