#include "kernel/task.h"

#include <kernel/arch/cpu.h>
#include <kernel/arch/irq.h>
#include <kernel/arch/mem.h>
#include <kernel/elf.h>
#include <kernel/io.h>
#include <kernel/string.h>
#include <kernel/sys/types.h>
#include <stddef.h>
#include <stdint.h>

#define TASK_TOTAL 64

static struct task init_task = {
    .priority = 1,
    .kernel_stack = (void *) MEM_VA_KERNEL_STACK,
};
static struct task *current_task = &init_task;
static struct task *tasks[TASK_TOTAL] = { &init_task, };
static pid_t task_count = 1;

void task_add(struct task *task) {
    for (size_t i = 0; i < TASK_TOTAL; i++) {
        if (!tasks[i]) {
            tasks[i] = task;
            break;
        }
    }
}

void task_block(pid_t pid) {
    for (size_t i = 0; i < TASK_TOTAL; i++) {
        if (tasks[i]->pid == pid) {
            tasks[i]->state = TASK_BLOCKED;
            tasks[i]->preempt_count--;
            break;
        }
    }

    task_schedule();
}

void task_unblock(pid_t pid) {
    for (size_t i = 0; i < TASK_TOTAL; i++) {
        if (tasks[i]->pid == pid) {
            tasks[i]->state = TASK_RUNNING;
            break;
        }
    }
}

struct task* task_current(void) {
    return current_task;
}

struct task_r_pid task_fork(void) {
    current_task->preempt_count++;

    struct mem_r_addr task_r = mem_get_kernel_page(MEM_RW);
    if (task_r.err != MEM_OK)
        return (struct task_r_pid) { .pid = -1, .err = TASK_ERR_MEM };

    struct mem_r_addr tran_table_r = mem_get_kernel_page(MEM_RW);
    if (tran_table_r.err != MEM_OK) {
        // TODO: Free task_r
        return (struct task_r_pid) { .pid = -1, .err = TASK_ERR_MEM };
    }

    struct mem_r_addr kernel_stack_r = mem_get_kernel_page(MEM_RW);
    if (kernel_stack_r.err != MEM_OK) {
        // TODO: Free task_r
        // TODO: Free tran_table_r
        return (struct task_r_pid) { .pid = -1, .err = TASK_ERR_MEM };
    }

    // Copy kernel stack
    void *kernel_stack = kernel_stack_r.addr;
    memcpy(kernel_stack, current_task->kernel_stack, PAGESIZE);

    struct task *child = task_r.addr;

    child->pid = ++task_count;
    child->address = current_task->address;
    child->user_stack = current_task->user_stack;
    child->kernel_stack = kernel_stack;
    child->parent = current_task;
    child->tran_table = MEM_VIRT_TO_PHYS(tran_table_r.addr);
    child->state = current_task->state;
    child->preempt_count = current_task->preempt_count;
    child->priority = current_task->priority;
    child->counter = child->priority;

    task_add(child);

    // Copy top level table
    if (current_task->tran_table) {
        memcpy(
            MEM_PHYS_TO_VIRT(child->tran_table),
            MEM_PHYS_TO_VIRT(current_task->tran_table),
            PAGESIZE
              );
    }

    // Store current context
    cpu_switch(current_task, current_task);

    // If child, return
    if (current_task == child) {
        current_task->preempt_count--;
        return (struct task_r_pid) { .pid = 0, .err = TASK_OK };
    }

    // If parent, copy stored context to new task
    memcpy(&child->context, &current_task->context, sizeof(current_task->context));
    // Setup child's kernel stack
    const uintptr_t ksp_offset = current_task->context.ksp - (uintptr_t) current_task->kernel_stack;
    child->context.ksp = (uintptr_t) child->kernel_stack + ksp_offset;

    // Mark task page tables as read only
    if (current_task->tran_table)
        mem_mark_copied(MEM_PHYS_TO_VIRT(current_task->tran_table));

    current_task->preempt_count--;
    return (struct task_r_pid) { .pid = child->pid, .err = TASK_OK };
}

void task_schedule(void) {
    long counter = -1;
    size_t task_index = 0;
    struct task *t = NULL;

    current_task->counter = 0;
    current_task->preempt_count++;

    for (size_t i = 0; i < TASK_TOTAL; i++) {
        t = tasks[i];
        if (!t)
            continue;

        t->counter += t->priority;

        if (t->state == TASK_RUNNING && t->counter > counter) {
            counter = t->counter;
            task_index = i;
        }
    }

    t = tasks[task_index];
    t->counter = t->priority;

    /* Switch task */
    if (current_task != t) {
        struct task *previous_task = current_task;
        current_task = t;
        cpu_switch(previous_task, current_task);
    }

    current_task->preempt_count--;
}

void task_tick(void) {
    current_task->counter--;
    if (current_task->counter > 0 || current_task->preempt_count > 0)
        return;

    irq_enable();
    task_schedule();
    irq_disable();
}
