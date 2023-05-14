#include "kernel/fork.h"

#include <stdint.h>

#include "boot/entry.h"
#include "kernel/mm.h"
#include "kernel/scheduler.h"

int8_t copy_process(uintptr_t function, uintptr_t arg) {
    preempt_disable();
    struct task *new_task;
    struct task *current = get_current_task();

    new_task = (struct task *) get_free_page();
    if (!new_task) { return 1; }

    new_task->priority = current->priority;
    new_task->state = TASK_RUNNING;
    new_task->counter = new_task->priority;
    new_task->preempt_count = 1;

    new_task->cpu_context.x19 = function;
    new_task->cpu_context.x20 = arg;
    new_task->cpu_context.pc = (uintptr_t) start_coroutine;

    new_task->cpu_context.sp = (uintptr_t) new_task + 4096;

    add_task(new_task);
    preempt_enable();
    return 0;
}

uint8_t move_to_user_mode(uintptr_t fn) {
    preempt_disable();

    struct task *current = get_current_task();
    struct task *new_task = (struct task *) get_free_page();
    if (!new_task) { return 1; }

    new_task->priority = current->priority;
    new_task->state = TASK_RUNNING;
    new_task->counter = new_task->priority;
    new_task->preempt_count = 1;

    new_task->cpu_context.x19 = fn;
    new_task->cpu_context.x20 = (uintptr_t) new_task + 4096;
    new_task->cpu_context.pc = (uintptr_t) start_user;
    new_task->cpu_context.sp = (uintptr_t) new_task + 4096;

    add_task(new_task);
    preempt_enable();
    return 0;
}
