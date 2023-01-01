#include "uart.h"

void main() {
    uart_init();
    uart_puts("Hello World!\n");

    while (1) {
        uart_send_char(uart_get_char());
    }
}
