#include "uart.h"

void uart_init() {
    uint32_t reg;

    reg = *GPFSEL1;
    reg &= ~(0b111 << 12); // Clean gpio14
    reg |= 0b10 << 12; // Set alt 5
    reg &= ~(0b111 << 15); // Clean gpio15
    reg |= 0b10 << 12; // Set alt 5
    *GPFSEL1 = reg;

    // Disable pull-up/down
    *GPPUD = 0;
    reg = 150;
    while (reg--) { asm volatile("nop"); }
    *GPPUDCLK0 = 1 << 14;
    *GPPUDCLK0 = 1 << 15;
    reg = 150;
    while (reg--) { asm volatile("nop"); }
    *GPPUDCLK0 = 0;

    *AUX_ENABLES |= 1; // Enable mini uart
    *AUX_MU_CNTL_REG = 0; // Disable receiver and transmitter
    *AUX_MU_IER_REG = 0x1;  // Enable interrupts
    *AUX_MU_LCR_REG = 0x7; // Enable 8-bit mode
    *AUX_MU_MCR_REG = 0; // Set RTS line high
    *AUX_MU_BAUD_REG = 270; // Set baud rate to 115200

    *AUX_MU_CNTL_REG = 0x3; // Enable receiver and transmitter
}

void uart_send_char(char c) {
    // Wait until transmitter is idle
    while (!(*AUX_MU_LSR_REG & 0x20)) {
        asm volatile("nop");
    }
    // Write the character to the buffer
    *AUX_MU_IO_REG = c;
}

/*
* Receive character via uart
*/
static char uart_get_char() {
    char c;
    // Wait until buffer isn't empty
    while (!(*AUX_MU_LSR_REG & 0x01)) {
        asm volatile("nop");
    }
    // Read it and return
    c = (char) (*AUX_MU_IO_REG);
    return c == '\r' ? '\n' : c;
}

void handle_uart_irq() {
    uart_send_char(uart_get_char());
    *AUX_MU_IIR_REG |= 0xff;
}

