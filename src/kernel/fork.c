#include "fork.h"
#include "mm.h"
#include "entry.h"
#include "scheduler.h"
#include "debug_printf.h"

int8_t copy_process(uint64_t function, uint64_t arg) {
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
    new_task->cpu_context.pc = (uint64_t) start_coroutine;

    new_task->cpu_context.sp = (uint64_t) new_task + 4096;

    add_task(new_task);
    preempt_enable();
    return 0;
}
