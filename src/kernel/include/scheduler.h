#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#define TOTAL_TASKS 64

#include <stdint.h>

/*
* Registers to be stored when switching tasks
*/
struct cpu_context {
    uintptr_t x19;
    uintptr_t x20;
    uintptr_t x21;
    uintptr_t x22;
    uintptr_t x23;
    uintptr_t x24;
    uintptr_t x25;
    uintptr_t x26;
    uintptr_t x27;
    uintptr_t x28;
    uintptr_t fp;
    uintptr_t sp;
    uintptr_t pc;
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

#endif
