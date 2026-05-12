#ifndef _KERNEL_TASK_H
#define _KERNEL_TASK_H

#include <stddef.h>

#include <kernel/arch/cpu.h>
#include <kernel/sys/types.h>
#include <stdint.h>

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
    struct cpu_context context; /* Stored CPU Context. DO NOT change position */
    uintptr_t* tran_table;      /* Top level translation table. DO NOT change position */
    pid_t pid;                  /* Process ID */
    void* address;              /* Page start address */
    void* user_stack;           /* Virtual address of user stack */
    void* kernel_stack;         /* Virtual address of kernel stack */
    struct task* parent;        /* Parent task */
    enum task_state state;      /* Current task state */
    int preempt_count;          /* If non-zero, task must not be interrupted */
    long priority;              /* Execution clock ticks given */
    long counter;               /* Execution clock ticks left */
};

enum task_err {
    TASK_OK,
    TASK_ERR_INVALID, // Invalid ELF file
    TASK_ERR_MEM,     // Out of memory
};

struct task_r_ptr {
    struct task *ptr;
    enum task_err err;
};

struct task_r_pid {
    pid_t pid;
    enum task_err err;
};

/*
* Add _task_ to currently executing tasks
*/
void task_add(struct task *task);

/*
* Block task with _pid_
*/
void task_block(pid_t pid);

/*
* Unblock task with _pid_
*/
void task_unblock(pid_t pid);

/*
* Get current running task
*/
struct task* task_current(void);

/*
* Fork currently executing task
*/
struct task_r_pid task_fork(void);

/*
* Initiate Round Robin scheduler
*/
void task_schedule(void);

/*
* De-increment task counter and call scheduler
*/
void task_tick(void);

#endif /* _KERNEL_TASK_H */
