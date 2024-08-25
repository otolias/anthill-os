#ifndef _TASK_H
#define _TASK_H

#define TOTAL_TASKS 64

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
    struct cpu_context cpu_context;     /* Stored CPU Context. DO NOT change position */
    pid_t              pid;             /* Process ID */
    void*              process_address; /* Page start address */
    void*              user_stack;      /* User stack address */
    void*              kernel_stack;    /* Kernel stack address */
    struct task*       parent;          /* Parent task */
    enum task_state    state;           /* Current task state */
    int                preempt_count;   /* If non-zero, task must not be interrupted */
    long               priority;        /* Execution clock ticks given */
    long               counter;         /* Execution clock ticks left */
};

/*
* Block current task
*/
void task_current_block(void);

/*
* Returns the process ID of the current task
*/
pid_t task_current_pid(void);

/*
* Load _file_ dependencies and execute with _argv_ arguments.
*
* On success, returns a pointer to the task.
* On failure, returns -errno.
*/
ssize_t task_exec(const void *file, char *const args[restrict]);

/*
* Terminate process
*/
void task_exit(void);

/*
* Initiate Round Robin scheduler
*/
void task_schedule(void);

/*
* De-increment task counter and call scheduler
*/
void task_tick(void);

/*
* Unblock task with _pid_
*/
void task_unblock(pid_t pid);

#endif /* _TASK_H */
