#include <kernel/task.h>

#include <stddef.h>

#include <boot/entry.h>
#include <drivers/irq.h>
#include <kernel/cpu_context.h>
#include <kernel/elf.h>
#include <kernel/kprintf.h>
#include <kernel/mm.h>
#include <kernel/string.h>
#include <kernel/ramdisk.h>

static struct task init_task = { .priority = 1};
struct task *current_task = &init_task;
struct task *tasks[TOTAL_TASKS] = {&init_task, };
unsigned int total_tasks = 1;

Elf64_Ehdr *linker_page;

void task_add(struct task* new_task) {
    tasks[++total_tasks] = new_task;
}

void task_switch(struct task *next) {
    if (current_task == next) return;

    struct task *previous = current_task;
    current_task = next;
    cpu_context_switch(previous, next);
}

void task_tick(void) {
    current_task->counter--;
    if (current_task->counter > 0 || current_task->preempt_count > 0) {
        return;
    }
    enable_irq();
    task_schedule();
    disable_irq();
}

/* Load _ehdr_ in _address_ */
static void _task_load(const Elf64_Ehdr *ehdr, const void *address) {
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

short task_exec(const void *file) {
    current_task->preempt_count++;

    /* Load linker if not already loaded*/
    if (!linker_page) {
        const Elf64_Ehdr *ehdr = (Elf64_Ehdr *) ramdisk_lookup("./lib/ld.so");

        /* Get pages */
        linker_page = get_free_pages(elf_get_image_size(ehdr));
        if (!linker_page) {
            kprintf("Error loading linker\n");
            return TASK_INIT_ERROR;
        };

        _task_load(ehdr, linker_page);
    }

    const Elf64_Ehdr *ehdr = (Elf64_Ehdr *) file;
    const unsigned long elf_memory_size = elf_get_image_size(ehdr);
    const unsigned long process_size = elf_memory_size + sizeof(struct task);

    const void *process_addr = get_free_pages(process_size);
    if (!process_addr) {
        kprintf("Error loading process\n");
        return TASK_INIT_ERROR;
    }

    _task_load(ehdr, process_addr);

    const size_t stack_offset = ELF_OFF(process_addr, elf_get_stack_offset(ehdr));

    /* Add process pages to stack */
    size_t *sp = (unsigned long *) stack_offset;
    *(sp--) = (size_t) file;
    *(sp--) = (size_t) process_addr;

    /* Create task struct */
    struct task *new_task = (struct task*) elf_memory_size;

    new_task->priority = current_task->priority;
    new_task->state = TASK_RUNNING;
    new_task->counter = new_task->priority;
    new_task->preempt_count = 1;

    new_task->cpu_context.x19 = (size_t) ELF_OFF(linker_page, linker_page->e_entry);
    new_task->cpu_context.x20 = (size_t) stack_offset;
    new_task->cpu_context.pc  = (size_t) start_user;
    new_task->cpu_context.sp  = (size_t) stack_offset;

    task_add(new_task);
    current_task->preempt_count--;
    return TASK_OK;
}

void task_schedule(void) {
    long next_task;
    struct task *p;

    current_task->counter = 0;
    current_task->preempt_count++;

    while(1) {
        long counter = -1;
        // Find the running process with the highest priority
        next_task = 0;

        for (size_t i = 0; i < TOTAL_TASKS; i++) {
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
        for (size_t i = 0; i < TOTAL_TASKS; i++) {
            p = tasks[i];
            if (p) {
                p->counter = (p->counter >> 1) + p->priority;
            }
        }
    }

    task_switch(tasks[next_task]);
    current_task->preempt_count--;
}
