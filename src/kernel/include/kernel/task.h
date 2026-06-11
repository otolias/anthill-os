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
    void* tran_table;           /* Top level translation table. DO NOT change position */
    pid_t pid;                  /* Process ID */
    void* kernel_stack;         /* Virtual address of kernel stack */
    struct task* parent;        /* Parent task */
    size_t children_no;         /* Number of running children */
    enum task_state state;      /* Current task state */
    int preempt_count;          /* If non-zero, task must not be interrupted */
    long priority;              /* Execution clock ticks given */
    long counter;               /* Execution clock ticks left */
};

enum task_err {
    TASK_OK,
    TASK_ERR_INV, // Invalid ELF file
    TASK_ERR_MEM, // Out of memory
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
* Add _task_ to task array
*/
void task_add(struct task *task);

/*
* Remove _task_ from task array
*/
void task_remove(struct task *task);

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
* Load ELF file pointed to by _file_ to memory and execute with _argc_
* arguments.
*
* Returns enum task_err.
*/
enum task_err task_exec(const void *file, char *const args[restrict]);

/*
* Terminate current running process.
*
* _status_ is not yet implemented.
*/
void task_exit(int status);

/*
* Fork currently executing task
*
* Returns struct task_r_pid:
* - On success, _pid_ is either the process ID of the child process (in the case
*   the caller) or 0 (in the case of the callee), and _err_ is set to TASK_OK.
* - On failure, _pid_ is -1 and _err_ is set to indicate the error.
*/
struct task_r_pid task_fork(void);

/*
* Run scheduler
*/
void task_schedule(void);

/*
* De-increment task counter and call scheduler
*/
void task_tick(void);

#endif /* _KERNEL_TASK_H */
