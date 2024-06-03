/*
* Length prefixed string for use with the plan 9 protocol
*/
#ifndef _PSTRING_H
#define _PSTRING_H

#include <sys/types.h>

typedef struct __attribute__((packed)) {
    unsigned short len;
    unsigned char  s[];
} pstring;

/*
* Converts the string pointed to by _src_ to a pstring representation
* stored in _dest_, with a maximum of _n_ bytes.
*
* If _dest_ is a null pointer, the pstring is malloc'd and _n_
* is ignored.
*
* On success, returns a pointer to the new pstring.
* On failure, returns a null pointer.
*/
pstring* pstrconv(void *dest, const char *src, size_t n);

/*
* Duplicate the pstring pointed to by _src_ to a new pstring stored in
* _dest_, with a maximum of _n_ bytes.
*
* If _dest_ is a null pointer, the pstring is malloc'd and _n_
* is ignored.
*
* On success, returns a pointer to the new pstring.
* On failure, returns a null pointer.
*/
pstring* pstrdup(void *dest, const pstring* src, size_t n);

/*
* Returns a pointer to the byte after _pstr_
*/
pstring* pstriter(const pstring *pstr);

/*
* Calculate the number of bytes in the pstring to which _s_ points to
*/
size_t pstrlen(const pstring* s);

/*
* Converts the pstring pointed to by _src_ to a nul-terminated string
* stored in _dest_, with a maximum of _n_ bytes.
*
* If _dest_ is a null pointer, the string is malloc'd and _n_ is ignored
*
* On success, returns a pointed to the new string.
* On failure, returns a null pointer.
*/
char* pstrtoz(char *dest, const pstring *src, size_t n);

#endif /* _PSTRING_H */
