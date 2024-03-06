/*
* Current implementation supports:
*
* Length modifier:   %l (not supported on %c and %s)
* Characters:        %c
* Signed Integers:   %d, %i
* Unsigned Integers: %u, %x
* Strings:           %s
*/

#ifndef _STDIO_H
#define _STDIO_H

#include <stdarg.h>

/*
* Put formatted string to _src_ with variable args.
*
* Returns the length of the resulting string
*/
int vsprintf(char *restrict s, const char *restrict format, va_list ap);
/*
* Put formatted string to _src_
*
* Returns the length of the resulting string
*/
int sprintf(char *restrict s, const char *restrict format, ...);
/*
* Send formatted string to uart
*
* Returns the length of the string
*/
int printf(const char *restrict format, ...);
/*
* Send string to uart
*
* Returns the length of the string
*/
int puts(const char *s);

#endif /* _STDIO_H */
