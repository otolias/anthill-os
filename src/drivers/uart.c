#include "uart.h"

void uart_init() {
    uint32_t reg;
    
    reg = *GFPSEL1;
    reg &= ~(0b111 << 12); // Clean gpio14
    reg |= 0b10 << 12; // Set alt 5
    reg &= ~(0b111 << 15); // Clean gpio15
    reg |= 0b10 << 12; // Set alt 5
    *GFPSEL1 = reg;

    // Disable pull-up/down
    *GPPUD = 0;
    reg = 150;
    while (reg--) { asm volatile("nop"); }
    *GPPUDCLK0 = 1 << 14;
    *GPPUDCLK0 = 1 << 15;
    reg = 150;
    while (reg--) { asm volatile("nop"); }
    *GPPUDCLK0 = 0;

    *AUX_ENABLES = 1; // Enable mini uart
    *AUX_MU_CNTL = 0; // Disable 
}
