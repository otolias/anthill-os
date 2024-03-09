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
#include <sys/types.h>

/*
* Put formatted string to _s_. Formatting is specified by _format_ and _ap_
*
* Returns the length of the resulting string, excluding the terminating
* null byte
*/
int vsprintf(char *restrict s, const char *restrict format, va_list ap);

/*
* Put formatted string to _s_. Formatting is specified by _format_ and
* a variable number of arguments
*
* Returns the length of the resulting string, excluding the terminating
* byte.
*/
int sprintf(char *restrict s, const char *restrict format, ...);

/*
* Put formatted string to _s_ up to _n_ bytes. Formatting is specified
* by _format_ and _ap_
*
* If the number of bytes to be written exceed _n_, a null byte is written
* and the rest are discarded
*
* Returns the number of bytes that would be written if _n_ had been
* large enough, excluding the terminating byte
*/
int vsnprintf(char *restrict s, size_t n, const char *restrict format, va_list ap);

/*
* Put formatted string to _s_ up to _n_ bytes. Formatting is specified
* by _format_ and a variable number of arguments
*
* If the number of bytes to be written exceed _n_, a null byte is written
* and the rest are discarded
*
* Returns the number of bytes that would be written if _n_ had been
* large enough, excluding the terminating byte
*/
int snprintf(char *restrict s, size_t n, const char *restrict format, ...);

/*
* Send formatted string to uart
*
* Returns the number of bytes transmitted
*/
int printf(const char *restrict format, ...);

/*
* Send string to uart
*
* Returns the number of bytes transmitted
*/
int puts(const char *s);

#endif /* _STDIO_H */
