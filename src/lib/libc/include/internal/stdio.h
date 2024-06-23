/*
* Internal stdio functions
*/

#ifndef _INTERNAL_STDIO_H
#define _INTERNAL_STDIO_H

#include <pstring.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

/* File handling */

/*
* Allocate buffer for _stream_.
*
* On success, returns a pointer to the allocated space.
* On failure, rreturns a null pointer and sets errno to indicate the failure.
*
* errno:
* - ENOMEM
*/
void *file_alloc(FILE *stream);

/*
* Disassociate fid of _stream_
*/
bool file_close(FILE *stream);

/*
* Close VFS connections.
*/
void file_deinit(void);

/*
* Get _stat_ info for stream associated with fid. Returns 0 on success and -1 on failure.
*/
int file_get_stat(unsigned fid, struct stat *stat);

/*
* Initialise VFS connections.
*/
bool file_init(void);

/*
* Open file for whose path name is equal to the one pointed to by _pathname_ with mode
* specified by _oflag_.
*
* On success, returns a pointer to the opened stream.
* On failure, returns a null pointer and sets errno to indicate the error.
*
* errno:
* - EACCES
* - EMFILE
*/
FILE* file_open(const char *pathname, int oflag);

/*
* Read _n_ bytes from _stream_.
*
* On success, returns the number of bytes read.
* On failure, returns 0 and sets errno to indicate the error.
*
* errno:
* - EIO
*/
unsigned file_read(FILE *stream, unsigned n);

/*
* Write _stream_ buffer to file.
*
* On success, returns the number of bytes written.
* On failure, returns EOF and sets errno to indicate the error.
*
* errno:
* - EBADF
* - EIO
*/
ssize_t file_write(FILE *stream);

/* Format handling */

/*
* Formats _format_ according to _args_ and puts the result in _buffer_,
* up to _max_size_ bytes
*/
int formatter(unsigned char* buffer, const char *format, va_list args, size_t max_size);

#endif /* _INTERNAL_STDIO_H */
