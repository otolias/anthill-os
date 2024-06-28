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
#define F_OPEN   1 << 0
#define F_EOF    1 << 1
#define F_READ   1 << 2
#define F_WRITE  1 << 3
#define F_APPEND 1 << 4
#define F_DIR    1 << 5

struct __file {
    unsigned       fid;    /* File descriptor */
    int            flags;  /* File status flags */
    off_t          offset; /* Seek offset */
    unsigned char* buf;    /* Stream buffer */
    unsigned char* r_pos;  /* Current read position */
    unsigned char* r_end;  /* End of read buffer */
    unsigned char* w_pos;  /* Current write position */
    unsigned char* w_end;  /* End of write buffer */
};

typedef struct __file FILE;

extern FILE __stdin;
extern FILE __stdout;
extern FILE __stderr;

#define stdin  &__stdin
#define stdout &__stdout
#define stderr &__stderr

extern FILE open_files[FOPEN_MAX];

/* File handling */

/*
* Flush and deallocate _stream_ buffer and disassociate _stream_ fid
*
* On success, returns 0.
* On failure, returns EOF and sets errno to indicate the error.
*
* errno:
* - EIO Physical I/O error. Data could not be written to file.
*/
int fclose(FILE *stream);

/*
* Write unwritten data in _stream_ buffer to the file.
*
* On success, returns 0.
* On failure, returns EOF and sets errno to indicate the error.
*
* errno:
* - EBADF _stream_ is not open for writing
* - EIO   Physical I/O error.
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
* - EBADF  The stream's underlying stream is nota  file descriptor open for reading.
* - EIO    Physical I/O error, or data could not be retrieved.
*/
int fgetc(FILE *stream);

/*
* Get the file descriptor associated with stream.
*
* On success, returns the file descriptor.
* On failure, returns -1 and sets errno to indicate the error.
*
* errno:
* - EBADF The stream is not associated with a file
*/
int fileno(FILE *stream);

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

/*
* Put up to _nitems_ of size _size_ to _ptr_ from _stream_.
*
* On success, returns the number of elements succesfully read.
* On failure, returns 0 and sets errno to indicate the error.
*
* errno:
* - EIO Physical I/O error, or data could not be retrieved.
*/
size_t fread(void *restrict ptr, size_t size, size_t nitems, FILE *restrict stream);

/*
* Write up to _nitems_ of size _size_ from the array pointed to by _ptr_ to _stream_.
* If _nitems_ or _size_ is 0, returns 0.
*
* On success, returns the number of elements written.
* On failure, returns 0 and sets errno to indicate the error.
*
* errno:
* - ENOMEM Not enough available space
*/
size_t fwrite(const void *restrict ptr, size_t size, size_t nitems,
              FILE *restrict stream);

/*
* Put bytes from _stream_ and a terminating character to _lineptr_ of size _n_,
* until _delimiter_ is encountered.
*
* _delimiter_ is an int, but it must be representable by an unsigned char.
* _lineptr_ must be a null pointer or a malloc'd buffer of size _n_.
*
* On success, returns the number of bytes writen to _lineptr_, excluding the
* terminating character.
* On failure, returns -1 and sets errno to indicate the error.
*
* errno:
* - EBADF  The stream's underlying stream is not a valid file descriptor.
* - EINVAL _lineptr_ or _n_ is a null pointer
* - EIO    Physical I/O error, or data could not be retrieved.
* - ENOMEM Not enough available space
*/
ssize_t getdelim(char **restrict lineptr, size_t *restrict n, int delimiter,
                 FILE *restrict stream);

/*
* Put bytes from _stream_ and a terminating character to _lineptr_ of size _n_,
* until a newline is encountered.
*
* _lineptr_ must be a null pointer or a malloc'd buffer of size _n_.
*
* On success, returns the number of bytes writen to _lineptr_, excluding the
* terminating character.
* On failure, returns -1 and sets errno to indicate the error.
*
* errno:
* - EBADF  The stream's underlying stream is not a valid file descriptor.
* - EINVAL _lineptr_ or _n_ is a null pointer
* - EIO    Physical I/O error, or data could not be retrieved.
* - ENOMEM Not enough available space
*/
ssize_t getline(char **restrict lineptr, size_t *restrict n, FILE *restrict stream);

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
