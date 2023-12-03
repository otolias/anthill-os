#ifndef _TASK_H
#define _TASK_H

#define TOTAL_TASKS 64

enum task_codes {
    TASK_OK,
    TASK_ELF_ERROR,
    TASK_INIT_ERROR,
};

/*
* Load _file_ dependencies and execute
*/
short task_exec(const void *file);

#endif /* _TASK_H */
