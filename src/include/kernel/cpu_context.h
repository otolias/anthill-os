#ifndef KERNEL_CPU_CONTEXT_
#define KERNEL_CPU_CONTEXT_

#include <stdint.h>

/*
* Registers to be stored when switching tasks
*/
struct cpu_context {
    // System Registers
    int64_t x19; int64_t x20; int64_t x21; int64_t x22; int64_t x23;
    int64_t x24; int64_t x25; int64_t x26; int64_t x27; int64_t x28;
    // SIMD Registers
    __int128_t v0; __int128_t v1; __int128_t v2; __int128_t v3; __int128_t v4;
    __int128_t v5; __int128_t v6; __int128_t v7; __int128_t v8; __int128_t v9;
    __int128_t v10; __int128_t v11; __int128_t v12; __int128_t v13; __int128_t v14;
    __int128_t v15; __int128_t v16; __int128_t v17; __int128_t v18; __int128_t v19;
    __int128_t v20; __int128_t v21; __int128_t v22; __int128_t v23; __int128_t v24;
    __int128_t v25; __int128_t v26; __int128_t v27; __int128_t v28; __int128_t v29;
    __int128_t v30; __int128_t v31;
    // Special purpose registers
    int64_t fp; int64_t sp; int64_t pc;
};

/*
* Switch cpu context from previous task to next
*/
void cpu_context_switch(void *previous, void *next);

#endif /* KERNEL_CPU_CONTEXT_ */
