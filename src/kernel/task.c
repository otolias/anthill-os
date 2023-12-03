#include <kernel/task.h>

#include <stddef.h>

#include <boot/entry.h>
#include <kernel/elf.h>
#include <kernel/kprintf.h>
#include <kernel/mm.h>
#include <kernel/scheduler.h>
#include <kernel/string.h>
#include <kernel/ramdisk.h>

Elf64_Ehdr *linker_page;

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
    preempt_disable();

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
    const struct task *current_task = get_current_task();
    struct task *new_task = (struct task*) elf_memory_size;

    new_task->priority = current_task->priority;
    new_task->state = TASK_RUNNING;
    new_task->counter = new_task->priority;
    new_task->preempt_count = 1;

    new_task->cpu_context.x19 = (size_t) ELF_OFF(linker_page, linker_page->e_entry);
    new_task->cpu_context.x20 = (size_t) stack_offset;
    new_task->cpu_context.pc  = (size_t) start_user;
    new_task->cpu_context.sp  = (size_t) stack_offset;

    add_task(new_task);
    preempt_enable();
    return TASK_OK;
}
