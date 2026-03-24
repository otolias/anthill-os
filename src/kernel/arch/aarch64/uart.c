#include "uart.h"

#include <stdint.h>
#include <kernel/arch/mem.h>

#include "mmio.h"

#define AUX_IRQ         ((volatile uint32_t*) (MEM_MMIO + 0x00215000))
#define AUX_ENABLES     ((volatile uint32_t*) (MEM_MMIO + 0x00215004))
#define AUX_MU_IO_REG   ((volatile uint32_t*) (MEM_MMIO + 0x00215040))
#define AUX_MU_IER_REG  ((volatile uint32_t*) (MEM_MMIO + 0x00215044))
#define AUX_MU_IIR_REG  ((volatile uint32_t*) (MEM_MMIO + 0x00215048))
#define AUX_MU_LCR_REG  ((volatile uint32_t*) (MEM_MMIO + 0x0021504C))
#define AUX_MU_MCR_REG  ((volatile uint32_t*) (MEM_MMIO + 0x00215050))
#define AUX_MU_LSR_REG  ((volatile uint32_t*) (MEM_MMIO + 0x00215054))
#define AUX_MU_MSR_REG  ((volatile uint32_t*) (MEM_MMIO + 0x00215058))
#define AUX_MU_SCRATCH  ((volatile uint32_t*) (MEM_MMIO + 0x0021505C))
#define AUX_MU_CNTL_REG ((volatile uint32_t*) (MEM_MMIO + 0x00215060))
#define AUX_MU_STAT_REG ((volatile uint32_t*) (MEM_MMIO + 0x00215064))
#define AUX_MU_BAUD_REG ((volatile uint32_t*) (MEM_MMIO + 0x00215068))

void uart_init(void) {
    volatile uint32_t reg;

    reg = *GPFSEL1;
    reg &= ~(0x7 << 12); // Clean gpio14
    reg |= 0x2 << 12; // Set alt 5
    reg &= ~(0x7 << 15); // Clean gpio15
    reg |= 0x2 << 12; // Set alt 5
    *GPFSEL1 = reg;

    // Disable pull-up/down
    *GPPUD = 0;
    reg = 150;
    while (reg--) { __asm__ volatile("nop"); }
    *GPPUDCLK0 = 1 << 14;
    *GPPUDCLK0 = 1 << 15;
    reg = 150;
    while (reg--) { __asm__ volatile("nop"); }
    *GPPUDCLK0 = 0;

    *AUX_ENABLES |= 1; // Enable mini uart
    *AUX_MU_CNTL_REG = 0; // Disable receiver and transmitter
    *AUX_MU_IER_REG = 0x1;  // Enable interrupts
    *AUX_MU_LCR_REG = 0x7; // Enable 8-bit mode
    *AUX_MU_MCR_REG = 0; // Set RTS line high
    *AUX_MU_BAUD_REG = 270; // Set baud rate to 115200

    *AUX_MU_CNTL_REG = 0x3; // Enable receiver and transmitter
}

unsigned uart_write_char(unsigned char c) {
    // Wait until transmitter is idle
    while (!(*AUX_MU_LSR_REG & 0x20))
        __asm__ volatile("nop");

    // Write the character to the buffer
    *AUX_MU_IO_REG = c;

    return 0;
}
