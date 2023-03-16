#include "stdio.h"
#include <stdarg.h>
#include <stdint.h>
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

void printf(char *format, ...) {
    va_list va;
    char c;
    char *string;
    char buffer[16];
    uint32_t i = 0;

    va_start(va, format);

    while ((c = *(format++))) {
        if (c != '%') {
            uart_send_char(c);
        } else {
            c = *(format++);

            switch (c) {
                case 'd':
                    integer_to_string(buffer, va_arg(va, uint32_t), 10);
                    for (i = 0; i < 16; i++) {
                        if (!buffer[i]) break;
                        uart_send_char(buffer[i]);
                        buffer[i] = '\x00';
                    }
                    break;
                case 'x':
                    integer_to_string(buffer, va_arg(va, uint32_t), 16);
                    for (i = 0; i < 16; i++) {
                        if (!buffer[i]) break;
                        uart_send_char(buffer[i]);
                        buffer[i] = '\x00';
                    }
                    break;
                case 's':
                    string = va_arg(va, char*);
                    while (*string) {
                        uart_send_char(*string);
                        string++;
                    }
                default:
                    break;
            }
        }
    }
    va_end(va);
}
