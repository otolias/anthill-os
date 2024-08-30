#include <kernel/task.h>

#include <boot/entry.h>
#include <drivers/irq.h>
#include <kernel/cpu_context.h>
#include <kernel/errno.h>
#include <kernel/elf.h>
#include <kernel/kprintf.h>
#include <kernel/mm.h>
#include <kernel/string.h>
#include <kernel/sys/types.h>
#include <kernel/ramdisk.h>

static struct task init_task = { .priority = 1 };
struct task *current_task = &init_task;
struct task *tasks[TOTAL_TASKS] = {&init_task, };
long task_count = 0;

Elf64_Ehdr *linker_page;

/* Load _ehdr_ in _address_ */
static void _load(const Elf64_Ehdr *ehdr, const void *address) {
    const Elf64_Phdr *phdr = (Elf64_Phdr *) ELF_OFF(ehdr, ehdr->e_phoff);
    size_t phdr_count;

    for (phdr_count = ehdr->e_phnum; phdr_count--; phdr++) {
        if (phdr->p_type != PT_LOAD && phdr->p_type != PT_DYNAMIC)
            continue;

        memcpy((void *) ELF_OFF(address, phdr->p_vaddr),
               (void *) ELF_OFF(ehdr, phdr->p_offset),
               phdr->p_filesz);

        /* Zero bss segment */
        const unsigned long size_diff = phdr->p_memsz - phdr->p_filesz;
        if (size_diff)
            memset((void *) (ELF_OFF(address, phdr->p_vaddr) + phdr->p_filesz), 0, size_diff);
    }
}

void task_current_block(void) {
    current_task->state = TASK_BLOCKED;
    current_task->preempt_count--;
    task_schedule();
}

pid_t task_current_pid(void) {
    return current_task->pid;
}

ssize_t task_exec(const void *file, char *const args[restrict]) {
    current_task->preempt_count++;

    /* Load linker if not already loaded */
    if (!linker_page) {
        const Elf64_Ehdr *ehdr = (Elf64_Ehdr *) ramdisk_lookup("./lib/ld.so");

        /* Get pages */
        linker_page = mm_get_pages(elf_get_image_size(ehdr));
        if (!linker_page) {
            kprintf("Error loading linker\n");
            return -EINVAL;
        };

        _load(ehdr, linker_page);
    }

    const Elf64_Ehdr *ehdr = (Elf64_Ehdr *) file;
    if (elf_validate(ehdr) != 0)
        return -ENOEXEC;

    const unsigned long elf_memory_size = elf_get_image_size(ehdr);
    const unsigned long process_size = elf_memory_size + sizeof(struct task);

    void *process_addr = mm_get_pages(process_size);
    if (process_addr == 0) {
        return -ENOMEM;
    }

    _load(ehdr, process_addr);

    void *user_stack = mm_get_pages(0);
    if (user_stack == NULL)
        { mm_free_pages(process_addr); return -ENOMEM; }

    void *kernel_stack = mm_get_pages(0);
    if (kernel_stack == NULL)
        { mm_free_pages(process_addr); mm_free_pages(user_stack); return -ENOMEM; }

    char *sp = (char *) user_stack + PAGE_SIZE;

    if (args != NULL) {
        /* Calculate argument size */
        int argc = 0;
        size_t arg_size = 0;
        while (1) {
            if (!args[argc]) break;
            arg_size += strlen(args[argc]) + 1;
            argc++;
        }

        /* Align arg_size */
        if (arg_size % 8)
            arg_size += (8 - arg_size % 8);

        /* Add arguments to stack */
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

    /* Add process pages to stack */
    sp -= sizeof(size_t);
    *((size_t *) sp) = (size_t) process_addr;
    sp -= sizeof(size_t);
    *((size_t *) sp) = (size_t) file;

    /* Create task struct */
    struct task *new_task = (struct task*) ELF_OFF(process_addr, elf_memory_size);

    new_task->pid = ++task_count;
    new_task->process_address = process_addr;
    new_task->user_stack = user_stack;
    new_task->kernel_stack = kernel_stack;
    new_task->state = TASK_RUNNING;
    new_task->priority = current_task->priority;
    new_task->counter = new_task->priority;
    new_task->preempt_count = 1;
    new_task->parent = current_task;

    new_task->cpu_context.x19 = (size_t) ELF_OFF(linker_page, linker_page->e_entry);
    new_task->cpu_context.x20 = (size_t) sp;
    new_task->cpu_context.pc = (size_t) start_user;
    new_task->cpu_context.ksp = (size_t) kernel_stack + PAGE_SIZE;

    /* Add task */
    for (size_t i = 0; i < TOTAL_TASKS; i++) {
        if (!tasks[i]) {
            tasks[i] = new_task;
            break;
        }
    }

    current_task->preempt_count--;
    return (ssize_t) new_task;
}

void task_exit(void) {
    for (size_t i = 0; i < TOTAL_TASKS; i++) {
        if (tasks[i] == current_task) {
            tasks[i] = NULL;
            break;
        }
    }

    task_unblock(current_task->parent->pid);
    mm_free_pages((void *) current_task->process_address);
    mm_free_pages((void *) current_task->user_stack);
    mm_free_pages((void *) current_task->kernel_stack);
    cpu_context_switch(current_task, &init_task);
}

void task_schedule(void) {
    long counter = -1;
    size_t task_index = 0;
    struct task *t = NULL;

    current_task->counter = 0;
    current_task->preempt_count++;

    for (size_t i = 0; i < TOTAL_TASKS; i++) {
        t = tasks[i];
        if (!t) continue;

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
        cpu_context_switch(previous_task, current_task);
    }

    current_task->preempt_count--;
}

void task_tick(void) {
    current_task->counter--;
    if (current_task->counter > 0 || current_task->preempt_count > 0)
        return;

    enable_irq();
    task_schedule();
    disable_irq();
}

void task_unblock(pid_t pid) {
    for (size_t i = 0; i < TOTAL_TASKS; i++) {
        if (tasks[i]->pid == pid) {
            tasks[i]->state = TASK_RUNNING;
            break;
        }
    }
}
