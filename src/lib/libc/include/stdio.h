/*
* Current implementation supports:
*
* Decimals:     %d
* Hexadecimals: %x
* Strings:      %s
*/

#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>

/*
* Put formatted string to _src_ with variable args.
*
* Returns the length of the resulting string
*/
int vsprintf(char *str, char *format, va_list args);
/*
* Put formatted string to _src_
*
* Returns the length of the resulting string
*/
int sprintf(char *str, char *format, ...);
/*
* Send formatted string to uart
*
* Returns the length of the string
*/
int printf(char *format, ...);

#endif /* _STDIO_H */
