#include "kernel/task.h"

#include <kernel/arch/cpu.h>
#include <kernel/arch/irq.h>
#include <kernel/arch/mem.h>
#include <kernel/elf.h>
#include <kernel/error.h>
#include <kernel/io.h>
#include <kernel/panic.h>
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

void task_remove(struct task *task) {
    for (size_t i = 0; i < TASK_TOTAL; i++) {
        if (tasks[i] == task) {
            tasks[i] = NULL;
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

enum kern_err task_exec(const void *file, char *const args[restrict]) {
    current_task->preempt_count++;

    const struct elf64_ehdr *ehdr = file;

    const enum kern_err err = elf_validate(ehdr);
    if (err != ERR_OK)
        return err;

    void *tran_table = MEM_PHYS_TO_VIRT(current_task->tran_table);

    // Unmark previous table entries
    if (mem_table_teardown(tran_table) != ERR_OK)
        panic("Failed to teardown table entries");

    const struct elf_r_addr stack_r = elf_create_proc_image(file, tran_table);
    if (stack_r.err != ERR_OK)
        return err;

    // Setup process arguments
    char *sp = (char *) stack_r.addr + PAGESIZE;

    if (args != NULL) {
        // Calculate argument size
        int argc = 0;
        size_t arg_size = 0;
        while (1) {
            if (!args[argc])
                break;
            arg_size += strlen(args[argc]) + 1;
            argc++;
        }

        // Align arg_size
        if (arg_size % 8)
            arg_size += (8 - arg_size % 8);

        // Add arguments to stack
        sp -= arg_size;
        char *arg_pos = sp;

        sp -= sizeof(char *) * (argc + 1);
        char **argv = (char **) sp;
        sp -= sizeof(size_t);
        *((size_t *) sp) = (size_t) argc;

        for (int i = 0; i < argc; i++) {
            size_t len = strlen(args[i]) + 1;
            memcpy(arg_pos, args[i], len);
            argv[i] = arg_pos;
            arg_pos += len;
        }

        argv[argc] = NULL;
    }

    current_task->preempt_count--;

    cpu_start_user(ehdr->e_entry, (uintptr_t) sp, current_task->tran_table);

    return ERR_OK;
}

void task_exit(__attribute__((unused)) int status) {
    // TODO: Pass status to parent
    current_task->preempt_count++;

    while (current_task->children_no > 0)
        task_block(current_task->pid);

    // Unblock parent
    current_task->parent->children_no--;
    task_unblock(current_task->parent->pid);

    // Traverse translation tables and free all memory
    if (mem_table_teardown(MEM_PHYS_TO_VIRT(current_task->tran_table)) != ERR_OK)
        panic("Failed to teardown table entries");

    // Free translation table
    if (mem_kernel_free_page(MEM_PHYS_TO_VIRT(current_task->tran_table)) != ERR_OK)
        panic("Failed to free translation table");

    // Free kernel stack
    if (mem_kernel_free_page(current_task->kernel_stack) != ERR_OK)
        panic("Failed to free kernel stack");

    // Remove from task array
    task_remove(current_task);

    // Free task struct
    if (mem_kernel_free_page(current_task) != ERR_OK)
        panic("Failed to free task struct");

    // Switch context to init_task
    current_task = &init_task;
    cpu_switch(NULL, &init_task);
}

struct task_r_pid task_fork(void) {
    current_task->preempt_count++;

    struct mem_r_addr task_r = mem_kernel_alloc_page(MEM_RW);
    if (task_r.err != ERR_OK)
        return (struct task_r_pid) { .pid = -1, .err = task_r.err };

    struct mem_r_addr tran_table_r = mem_kernel_alloc_page(MEM_RW);
    if (tran_table_r.err != ERR_OK) {
        // TODO: Free task_r
        return (struct task_r_pid) { .pid = -1, .err = tran_table_r.err };
    }

    struct mem_r_addr kernel_stack_r = mem_kernel_alloc_page(MEM_RW);
    if (kernel_stack_r.err != ERR_OK) {
        // TODO: Free task_r
        // TODO: Free tran_table_r
        return (struct task_r_pid) { .pid = -1, .err = kernel_stack_r.err };
    }

    // Copy kernel stack
    memcpy(kernel_stack_r.addr, current_task->kernel_stack, PAGESIZE);

    struct task *child = (struct task *) task_r.addr;

    child->pid = ++task_count;
    child->kernel_stack = kernel_stack_r.addr;
    child->parent = current_task;
    child->children_no = 0;
    child->tran_table = MEM_VIRT_TO_PHYS(tran_table_r.addr);
    child->state = current_task->state;
    child->preempt_count = current_task->preempt_count;
    child->priority = current_task->priority;
    child->counter = child->priority;

    current_task->children_no++;

    task_add(child);

    // Mark task page tables as read only
    mem_table_soft_copy(MEM_PHYS_TO_VIRT(current_task->tran_table));

    // Copy top level table
    memcpy(tran_table_r.addr, MEM_PHYS_TO_VIRT(current_task->tran_table), PAGESIZE);

    // Store current context
    cpu_switch(current_task, NULL);

    // If child, return
    if (current_task == child) {
        current_task->preempt_count--;
        return (struct task_r_pid) { .pid = 0, .err = ERR_OK };
    }

    // If parent, copy stored context to new task
    memcpy(&child->context, &current_task->context, sizeof(current_task->context));

    // Setup child's kernel stack
    const uintptr_t ksp_offset = current_task->context.ksp - (uintptr_t) current_task->kernel_stack;
    child->context.ksp = (uintptr_t) child->kernel_stack + ksp_offset;

    current_task->preempt_count--;
    return (struct task_r_pid) { .pid = child->pid, .err = ERR_OK };
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
