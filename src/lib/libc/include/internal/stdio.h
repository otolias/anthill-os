/*
* Internal stdio functions
*/

#ifndef _INTERNAL_STDIO_H
#define _INTERNAL_STDIO_H

#include <pstring.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/types.h>

#define PRINTF_BUFFER_SIZE 1024

/* File handling */

/*
* Open file for whose path name is pointed to by _pathname_ with
* mode specified by _oflag_
*/
FILE* file_open(const char *pathname, int oflag);

/* Format handling */

/*
* Formats _format_ according to _args_ and puts the result in _buffer_,
* up to _max_size_ bytes
*/
int formatter(char* buffer, const char *format, va_list args, size_t max_size);

#endif /* _INTERNAL_STDIO_H */
