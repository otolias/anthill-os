#include <stdint.h>

/*
* Initialise uart
*/
void uart_init();
/*
* Send character via uart
*/
void uart_send(uint32_t c);
/*
* Receive character via uart
*/
char uart_get_char();
/*
* Send string via uart
*/
void uart_puts(char *string);
