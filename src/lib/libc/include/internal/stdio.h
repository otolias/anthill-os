/*
* Internal stdio formatting functions
*/

#ifndef _INTERNAL_STDIO_H
#define _INTERNAL_STDIO_H

#include <stdarg.h>

#define PRINTF_BUFFER_SIZE 1024

/*
* Formats _format_ according to _args_ and puts the result in _buffer_
*/
int _formatter(char* buffer, const char *format, va_list args);

/*
* Puts _c_ in _loc_ of _buffer_
*/
int _parse_char(unsigned char c, char *buffer, int loc);

/*
* Puts _val_ with _base_ in location _loc_ of _buffer_
*
* Returns new location after application
*/
int _parse_int(long int val, const int base, char* buffer, int loc);

/*
* Puts _string_ in _loc_ of _buffer_
*
* Returns new location after application
*/
int _parse_string(char *string, char* buffer, int loc);

#endif /* _INTERNAL_STDIO_H */
