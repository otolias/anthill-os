#ifndef _FORK_H
#define _FORK_H

#include <stdint.h>

/*
* Copies current process context and starts a new one
* with the specified argument
*/
int8_t copy_process(uintptr_t function, uintptr_t arg);
/*
* Starts the given function in EL0
*/
uint8_t move_to_user_mode(uintptr_t fn);

#endif
