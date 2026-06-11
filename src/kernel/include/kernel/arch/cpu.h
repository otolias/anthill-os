#ifndef _KERNEL_ARCH_AARCH64_CPU_H
#define _KERNEL_ARCH_AARCH64_CPU_H

#include <stddef.h>
#include <stdint.h>

struct task;

/*
* Registers to be stored when switching tasks
*/
struct cpu_context {
    /* System Registers */
    int64_t x19; int64_t x20; int64_t x21; int64_t x22; int64_t x23;
    int64_t x24; int64_t x25; int64_t x26; int64_t x27; int64_t x28;
    /* SIMD Registers */
    __int128_t v0; __int128_t v1; __int128_t v2; __int128_t v3; __int128_t v4;
    __int128_t v5; __int128_t v6; __int128_t v7; __int128_t v8; __int128_t v9;
    __int128_t v10; __int128_t v11; __int128_t v12; __int128_t v13; __int128_t v14;
    __int128_t v15; __int128_t v16; __int128_t v17; __int128_t v18; __int128_t v19;
    __int128_t v20; __int128_t v21; __int128_t v22; __int128_t v23; __int128_t v24;
    __int128_t v25; __int128_t v26; __int128_t v27; __int128_t v28; __int128_t v29;
    __int128_t v30; __int128_t v31;
    /* Special purpose registers */
    int64_t fp; uint64_t sp; int64_t pc;
    /* Kernel stack pointer */
    uint64_t ksp;
};

/*
 * Switches execution to user mode at address _entry_, sets stack pointer to
 * address _sp_ and changes translation table to tran_table.
*/
[[noreturn]] void cpu_start_user(uintptr_t entry, uintptr_t sp, void *tran_table);

/*
* Store current cpu context to task pointed to by _prev_ and restore cpu context
* from task pointed to by _next_. If _prev_ is NULL, only restores _next_ and
* if _next_ is NULL, only stores _prev_.
*/
void cpu_switch(struct task *prev, struct task *next);

#endif /* _KERNEL_ARCH_AARCH64_CPU_H */
