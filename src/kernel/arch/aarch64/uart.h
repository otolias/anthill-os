#ifndef _KERNEL_ARCH_AARCH64_UART_H
#define _KERNEL_ARCH_AARCH64_UART_H

/*
* Initialise UART device
*/
void uart_init();

/*
* Write _c_ to UART device
*
* On success, returns 0
*/
unsigned uart_write_char(unsigned char c);

#endif /* _KERNEL_ARCH_AARCH64_UART_H */
