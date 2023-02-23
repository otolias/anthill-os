#include "scheduler.h"
#include "irq.h"

static struct task init_task = { {0,0,0,0,0,0,0,0,0,0,0,0,0}, TASK_RUNNING, 0, 1, 0};
struct task *current = &init_task;
struct task *tasks[TOTAL_TASKS] = {&init_task, };
uint32_t total_tasks = 1;

struct task* get_current_task() {
    return current;
}

void add_task(struct task* new_task) {
    tasks[++total_tasks] = new_task;
}

void preempt_disable() {
    current->preempt_count++;
}

void preempt_enable() {
    current->preempt_count--;
}

void switch_to(struct task *next) {
    if (current == next) return;

    struct task *previous = current;
    current = next;
    switch_context(previous, next);
}

void schedule() {
    int64_t next_task, counter;
    struct task *p;

    current->counter = 0;
    preempt_disable();

    while(1) {
        // Find the running process with the highest priority
        counter = -1;
        next_task = 0;

        for (uint32_t i = 0; i < TOTAL_TASKS; i++) {
            p = tasks[i];
            if (p && p->state == TASK_RUNNING && p->counter > counter) {
                counter = p->counter;
                next_task = i;
            }
        }

        // If such a process exists, switch to it
        if (counter) {
            break;
        }

        // If not, update task counters
        for (int i = 0; i < TOTAL_TASKS; i++) {
            p = tasks[i];
            if (p) {
                p->counter = (p->counter >> 1) + p->priority;
            }
        }
    }

    switch_to(tasks[next_task]);
    preempt_enable();
}

void timer_tick() {
    current->counter--;
    if (current->counter > 0 || current->preempt_count > 0) {
        return;
    }
    enable_irq();
    schedule();
    disable_irq();
}
