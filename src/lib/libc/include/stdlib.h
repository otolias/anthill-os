#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/types.h>

/*
* Terminate process
*
* Status is currently unsupported
*/
void exit(int status);

/*
* Deallocate space pointed to by _ptr_
*/
void free(void *ptr);

/*
* Allocate unused space for an object of _size_ in bytes
*
* On success, returns a pointer to the allocated space.
* On failure, returns a null pointer and sets errno to
* indicate the error
*
* errno:
* - ENOMEM Not enough available storage
*/
void* malloc(size_t size);

/*
* Convert the initial portion of the string pointed
* to by _str_ to a type long representation in radix
* _base_
*
* Only decimals are currently supported
*
* On success, returns the converted value and sets
* _endptr_ to point to the rest of the string, if
* not null
* On failure, returns 0 and sets errno to indicate
* the error
*
* errno:
* - EINVAL The value of _base_ is not supported, or the
*          value could not be converted
* - ERANGE The value to be returned is not representable
*/
long strtol(const char *restrict str, char **restrict endptr, int base);

/*
* Convert the initial portion of the string pointed
* to by _str_ to a type long long representation in
* radix _base_
*
* Only decimals are currently supported
*
* On success, returns the converted value and sets
* _endptr_ to point to the rest of the string, if
* not null
* On failure, returns 0 and sets errno to indicate
* the error
*
* errno:
* - EINVAL The value of _base_ is not supported, or the
*          value could not be converted
* - ERANGE The value to be returned is not representable
*/
long long strtoll(const char *restrict nptr, char **restrict endptr, int base);

/*
* Convert the initial portion of the string pointed
* to by _str_ to a type unsigned long representation
* in radix _base_
*
* Only decimals are currently supported
*
* On success, returns the converted value and sets
* _endptr_ to point to the rest of the string, if
* not null
* On failure, returns 0 and sets errno to indicate
* the error
*
* errno:
* - EINVAL The value of _base_ is not supported, or the
*          value could not be converted
* - ERANGE The value to be returned is not representable
*/
unsigned long strtoul(const char *restrict str, char **restrict endptr, int base);

/*
* Convert the initial portion of the string pointed
* to by _str_ to a type unsigned long long representation
* in radix _base_
*
* Only decimals are currently supported
*
* On success, returns the converted value and sets
* _endptr_ to point to the rest of the string, if
* not null
* On failure, returns 0 and sets errno to indicate
* the error
*
* errno:
* - EINVAL The value of _base_ is not supported, or the
*          value could not be converted
* - ERANGE The value to be returned is not representable
*/
unsigned long long strtoull(const char *restrict str, char **restrict endptr, int base);

#endif /* _STDLIB_H */
