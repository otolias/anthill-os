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
int formatter(char* buffer, const char *format, va_list args);

#endif /* _INTERNAL_STDIO_H */
