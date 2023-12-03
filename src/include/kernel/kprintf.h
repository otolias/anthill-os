/*
* Small printf for kernel usage. Supports decimals, hexadecimals and strings
*/
#ifndef _K_PRINTF
#define _K_PRINTF

#ifdef DEBUG
#define debug_log(...) kprintf(__VA_ARGS__)
#else
#define debug_log(...)
#endif /* DEBUG */

/*
* Write formatted string to _str_
*/
int ksprintf(char *str, char *format, ...);

/*
* Send formatted string to UART
*/
void kprintf(char *format, ...);

#endif /* _K_PRINTF */
