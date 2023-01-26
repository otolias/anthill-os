#ifndef _DBG_PRINTF_H
#define _DBG_PRINTF_H

/*
* A very basic implementation of printf targeting uart
* for debugging purposes
*/

/*
* Send formatted string to uart
*
* Supports only decimal, hexadecimal and character
* formatting options.
*/
void dbg_printf(char *format, ...);

#endif
