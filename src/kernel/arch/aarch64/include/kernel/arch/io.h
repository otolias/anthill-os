#ifndef _KERNEL_ARCH_AARCH64_IO_H
#define _KERNEL_ARCH_AARCH64_IO_H

/*
* Setup serial I/O device
*/
void io_init(void);

/*
* Write _n_ bytes of _data_ to serial I/O device
*/
int io_write(const unsigned char *data, int n);

#endif /* _KERNEL_ARCH_AARCH64_IO_H */
