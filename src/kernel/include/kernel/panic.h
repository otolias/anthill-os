#ifndef _KERNEL_PANIC_H
#define _KERNEL_PANIC_H

#define panic(m) _panic(m, __FILE__, __LINE__, __func__)

/*
* Write _msg, _file_, _line_ and _func_ and hang indefinitely.
*/
[[noreturn]] void _panic(const char *msg, const char *file, int line, const char *func);

#endif /* _KERNEL_PANIC_H */
