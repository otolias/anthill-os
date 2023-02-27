#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#define TOTAL_TASKS 64

#include <stdint.h>

#ifndef int128_t
#define int128_t __uint128_t
#endif /* int128_t */

/*
* Registers to be stored when switching tasks
*/
struct cpu_context {
    // System Registers
    uint64_t x19; uint64_t x20; uint64_t x21; uint64_t x22; uint64_t x23;
    uint64_t x24; uint64_t x25; uint64_t x26; uint64_t x27; uint64_t x28;
    // SIMD Registers
    int128_t v0; int128_t v1; int128_t v2; int128_t v3; int128_t v4;
    int128_t v5; int128_t v6; int128_t v7; int128_t v8; int128_t v9;
    int128_t v10; int128_t v11; int128_t v12; int128_t v13; int128_t v14;
    int128_t v15; int128_t v16; int128_t v17; int128_t v18; int128_t v19;
    int128_t v20; int128_t v21; int128_t v22; int128_t v23; int128_t v24;
    int128_t v25; int128_t v26; int128_t v27; int128_t v28; int128_t v29;
    int128_t v30; int128_t v31;
    // Special purpose registers
    uint64_t fp; uint64_t sp; uint64_t pc;
};

/*
* Task states
*/
enum task_state {
    TASK_RUNNING
};

/*
* Task descriptor
*/
struct task {
    struct cpu_context cpu_context;
    int64_t counter; // How long the task has been running (decreases)
    int64_t priority; // How much time the task is given
    int32_t preempt_count; // If non-zero, task must not be interrupted
    enum task_state state; // Current task state
};

/*
* Returns currently executing task
*/
struct task* get_current_task();
/*
* Add a new task
*/
void add_task(struct task* new_task);
/*
* Decrease current task's preempt count
*/
void preempt_disable();
/*
* Increase current task's preempt count
*/
void preempt_enable();
/*
* Switch to given task
*/
void switch_to(struct task *next);
/*
* Round Robin scheduler
*/
void schedule();
/*
* Called by timer interrupt. Calls scheduler
*/
void timer_tick();
/*
* Switch cpu context from previous task to next
*/
void switch_context(struct task *previous, struct task *next);

#endif /* _SCHEDULER_H */
