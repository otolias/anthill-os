#ifndef _KERNEL_IO_H
#define _KERNEL_IO_H

/*
* Output formatted string specified by _format_. Returns the number of bytes
* written.
*/
int io_fmt(const char *restrict format, ...);

#endif /* _KERNEL_IO_H */
