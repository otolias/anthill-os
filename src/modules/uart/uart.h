#ifndef _MOD_UART_H
#define _MOD_UART_H

/*
* Put up to _n_ bytes to _data_ from uart.
*
* Returns the number of bytes written.
*/
unsigned uart_read(unsigned char *data, unsigned n);

/*
* Write _n_ bytes of _data_ to uart
*/
unsigned uart_write(const unsigned char* data, unsigned n);

#endif /* _MOD_UART_H */
