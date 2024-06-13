/*
* Current formatting implementation supports:
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

#define BUFSIZ 512
#define FOPEN_MAX 16

#define EOF -1

/* File flag offsets */
#define F_OPEN 0 << 0
#define F_EOF  1 << 1

typedef struct {
    unsigned fid;         /* File descriptor */
    int      flags;       /* File status flags */
    size_t   seek_offset; /* Seek offset */
    ssize_t  chunk_index; /* Current buffer chunk index */
    char*    buf;         /* Stream buffer */
    char*    buf_pos;     /* Current buffer position */
    char*    buf_end;     /* Pointer to end of buffer */
} FILE;

extern FILE __stdin;
extern FILE __stdout;
extern FILE __stderr;

#define stdin  &__stdin
#define stdout &__stdout
#define stderr &__stderr

/* File handling */

/*
* Write unwritten data in _stream_ buffer to the file.
*
* On success, returns 0.
* On failure, returns EOF and sets errno to indicate the error.
*
* errno:
* - EIO Physical I/O error.
*/
int fflush(FILE *stream);

/*
* Get the next byte of _stream_.
*
* On success, returns the next byte as an int, or EOF if at end of file.
* On failure, returns EOF and sets errno to indicate the error.
*
* errno:
* - ENOMEM Not enough available space.
* - EBADF  The stream's underlying stream is not a valid file descriptor.
* - EIO    Physical I/O error, or data could not be retrieved.
*/
int fgetc(FILE *stream);

/*
* Open the file whose path name is the string pointed to by _pathname_ with
* the indicated _mode_.
*
* On success, returns a pointer to the FILE object controlling the stream.
* On failure, returns a null pointer and sets errno to indicate the error.
*
* Available modes:
* - r  Open file for reading
* - w  Truncate or create file for writing
* - a  Append or create file
* - r+ Open file for reading and writing
* - w+ Truncate or create file for reading and writing
* - a+ Append or create file for reading or writing
*
* errno:
* - EACCES File not found
* - EMFILE No available file descriptors
*/
FILE* fopen(const char *restrict pathname, const char *restrict mode);

/*
* Write _c_ to _stream_
*
* On success, returns the value as written.
* On failure, returns EOF and sets errno to indicate the error.
*
* errno:
* - ENOMEM Not enough available space
* - EIO    Buffer of _stream_ must be flushed
*/
int fputc(int c, FILE *stream);

/* Format handling */

/*
* Put string specified by _format_ and a variable number of arguments to stdout.
*
* On success, returns the number of bytes written to stdout.
* On failure, returns EOF and sets errno to indicate the error.
*
* errno:
* - EIO Physical I/O error, or the associated buffer isn't large enough
*/
int printf(const char *restrict format, ...);

/*
* Put the string pointed to by _s_, followed by a newline (\n) to stdout.
*
* On success, returns the number of bytes written to stdout.
* On failure, returns EOF and sets errno to indicate the error.
*
* errno:
* - EIO Physical I/O error, or the associated buffer isn't large enough.
*/
int puts(const char *s);

/*
* Put string specified by _format_ and a variable number of arguments to buffer
* associated with _stream_. The associated buffer may be flushed.
*
* On success, returns the number of bytes written to the buffer.
* On failure, returns EOF and sets errno to indicate the error.
*
* errno:
* - ENOMEM Not enough available space
* - EIO    Physical I/O error, or the associated buffer isn't large enough
*/
int fprintf(FILE *restrict stream, const char *restrict format, ...);

/*
* Put the string pointed to by _s_ to _stream_.
*
* On success, returns the number of bytes written.
* On failure, returns EOF and sets errno to indicate the error.
*
* errno:
* - ENOMEM Not enough available space
* - EIO    Physical I/O error, or the associated buffer isn't large enough
*/
int fputs(const char *restrict s, FILE *restrict stream);

/*
* Put nul-terminated string specified by _format_ and a variable number of arguments
* to _s_ up to _n_ bytes.
*
* If the number of bytes to be written exceed _n_, a null byte is written
* and the rest are discarded
*
* Returns the number of bytes that would be written if _n_ had been
* large enough, excluding the terminating byte
*/
int snprintf(char *restrict s, size_t n, const char *restrict format, ...);

/*
* Put nul-terminated string specified by _format_ and a variable number of arguments
* to _s_.
*
* Returns the length of the resulting string, excluding the terminating
* byte.
*/
int sprintf(char *restrict s, const char *restrict format, ...);

/*
* Put string specified by _format_ and _ap_ to buffer associated with _stream_.
* The associated buffer may be flushed.
*
* On success, returns the number of bytes written to the buffer.
* On failure, returns EOF and sets errno to indicate the error.
*
* errno:
* - ENOMEM Not enough available space
* - EIO    Physical I/O error, or the associated buffer isn't large enough
*/
int vfprintf(FILE *restrict stream, const char *restrict format, va_list ap);

/*
* Put string specified by _format_ and _ap_ to stdout.
*
* On success, returns the number of byte written to stdout.
* On failure, returns EOF and sets errno to indicate the error.
*
* errno:
* - EIO Physical I/O error, or the associated buffer isn't large enough
*/
int vprintf(const char *restrict format, va_list ap);

/*
* Put nul-terminated string specified by _format_ and _ap_ to _s_ up to _n_ bytes.
*
* If the number of bytes to be written exceed _n_, a null byte is written
* and the rest are discarded
*
* Returns the number of bytes that would be written if _n_ had been
* large enough, excluding the terminating byte
*/
int vsnprintf(char *restrict s, size_t n, const char *restrict format, va_list ap);

/*
* Put nul-terminated string specified by _format_ and _ap_ to _s_.
*
* Returns the length of the resulting string, excluding the terminating
* byte
*/
int vsprintf(char *restrict s, const char *restrict format, va_list ap);

#endif /* _STDIO_H */
