#include "stdio.h"
#include <stdarg.h>
#include <stdint.h>
#include "uart.h"

#define IO_BUFFER_SIZE     1024
#define LOCAL_BUFFER_SIZE  64

char io_buffer[IO_BUFFER_SIZE];
uint32_t io_buffer_index = 0;

/*
* Send buffer contents to uart
*/
void flush_io_buffer() {
    for (int i = 0; i < IO_BUFFER_SIZE; i++) {
        uart_send_char(io_buffer[i]);
        io_buffer[i] = '\x00';
    }

    io_buffer_index = 0;
}

/*
* Append character to io_buffer. If the buffer is full, call flush_io_buffer
*/
void append_to_io_buffer(char c) {
    if (io_buffer_index == IO_BUFFER_SIZE) 
        flush_io_buffer();

    io_buffer[io_buffer_index++] = c;
}

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
    char buffer[LOCAL_BUFFER_SIZE];
    uint32_t i = 0;

    va_start(va, format);

    while ((c = *(format++))) {
        if (c != '%') {
            append_to_io_buffer(c);
        } else {
            c = *(format++);

            switch (c) {
                case 'd':
                    integer_to_string(buffer, va_arg(va, uint32_t), 10);
                    for (i = 0; i < LOCAL_BUFFER_SIZE; i++) {
                        if (!buffer[i]) break;
                        append_to_io_buffer(buffer[i]);
                        buffer[i] = '\x00';
                    }
                    break;
                case 'x':
                    integer_to_string(buffer, va_arg(va, uint32_t), 16);
                    for (i = 0; i < LOCAL_BUFFER_SIZE; i++) {
                        if (!buffer[i]) break;
                        append_to_io_buffer(buffer[i]);
                        buffer[i] = '\x00';
                    }
                    break;
                case 's':
                    string = va_arg(va, char*);
                    while (*string) {
                        append_to_io_buffer(*string);
                        string++;
                    }
                default:
                    break;
            }
        }
    }

    flush_io_buffer();        

    va_end(va);
}
