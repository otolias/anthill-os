#ifndef _TASK_H
#define _TASK_H

#define TOTAL_TASKS 64
#define STACK_SIZE  4096

#include <kernel/cpu_context.h>
#include <kernel/sys/types.h>

/*
* Task states
*/
enum task_state {
    TASK_RUNNING,
    TASK_BLOCKED,
};

/*
* Task descriptor
*/
struct task {
    struct cpu_context cpu_context; /* Stored CPU context. Do not change position */
    pid_t pid;                      /* process ID */
    size_t process_address;         /* Page start address */
    size_t user_stack;              /* User stack address */
    size_t kernel_stack;            /* Kernel stack address */
    long counter;                   /* How long the task has been running (decreases) */
    long priority;                  /* How much time the task is given */
    int preempt_count;              /* If non-zero, task must not be interrupted */
    enum task_state state;          /* Current task state */
};

/*
* Returns the process ID of the current task
*/
pid_t task_current_pid(void);

/*
* Block current task
*/
void task_current_block(void);

/*
* Unblock task with _pid_
*/
void task_unblock(pid_t pid);

/*
* De-increment task counter and call scheduler
*/
void task_tick(void);

/*
* Initiate round Robin scheduler
*/
void task_schedule(void);

/*
* Load _file_ dependencies and execute. Returns a pointer to the process address
*/
const void* task_exec(const void *file);

/*
* Terminate process
*/
void task_exit(void);

#endif /* _TASK_H */
