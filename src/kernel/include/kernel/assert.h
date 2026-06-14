#ifndef _KERNEL_ASSERT_H
#define _KERNEL_ASSERT_H

#include <kernel/panic.h>

#define assert(x) ((void) ((x) || (_panic("Assertion failed [[ "#x " ]]", __FILE__, __LINE__, __func__), 0)))

#endif /* _KERNEL_ASSERT_H */
