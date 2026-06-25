#ifndef _CTYPE_H
#define _CTYPE_H

/*
* Test whether _c_ is a digit
*
* On success, returns 1
* On failure, returns 0
*/
int isdigit(int c);

/*
* Test whether _c_ is a white-space character
*
* White-space characters are considered:
* - space           (' ')
* - form-feed       ('\f')
* - newline         ('\n')
* - carriage-return ('\r')
* - horizontal tab  ('\t')
* - vertical tab    ('\v')
*
* On success, returns 1
* On failure, returns 0
*/
int isspace(int c);

#endif /* _CTYPE_H */
