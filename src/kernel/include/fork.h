#ifndef _FORK_H
#define _FORK_H

#include <stdint.h>

/*
* Copies current process context and starts a new one
* with the specified argument
*/
int8_t copy_process(uint64_t function, uint64_t arg);

#endif
