#ifndef _TASK_H
#define _TASK_H

#define TOTAL_TASKS 64

#include <kernel/cpu_context.h>

/*
* Task states
*/
enum task_state {
    TASK_RUNNING
};

enum task_error_codes {
    TASK_OK,
    TASK_ELF_ERROR,
    TASK_INIT_ERROR,
};

/*
* Task descriptor
*/
struct task {
    struct cpu_context cpu_context;
    long counter; // How long the task has been running (decreases)
    long priority; // How much time the task is given
    int preempt_count; // If non-zero, task must not be interrupted
    enum task_state state; // Current task state
};

/*
* Add a new task
*/
void task_add(struct task* new_task);

/*
* Switch to given task
*/
void task_switch(struct task *next);

/*
* De-increment task counter and call scheduler
*/
void task_tick(void);

/*
* Initiate round Robin scheduler
*/
void task_schedule(void);

/*
* Load _file_ dependencies and execute
*/
short task_exec(const void *file);

#endif /* _TASK_H */
