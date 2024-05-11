/*
* Length prefixed string for use with the plan 9 protocol
*/
#ifndef _PSTRING_H
#define _PSTRING_H

#include <sys/types.h>

typedef struct __attribute__((packed)) {
    unsigned short len;
    char           s[];
} pstring;

/*
* Calculate the number of bytes in the pstring to which _s_ points to
*/
size_t pstrlen(const pstring* s);

#endif /* _PSTRING_H */
