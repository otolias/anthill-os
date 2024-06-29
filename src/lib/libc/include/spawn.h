#ifndef _SPAWN_H
#define _SPAWN_H

#include <sys/types.h>

typedef struct {} posix_spawn_file_actions_t;
typedef struct {} posix_spawnattr_t;

/*
* Create a child process from the process image specified by _path_ with _argv_
* arguments.
*
* _file_actions_, _attrp_ and _envp_ are currently ignored.
*
* On success, sets the pid of the new process to _pid_ (if not a null pointer) and
* returns 0.
* On failure, returns -1 and sets errno to indicate the error.
*
* errno:
* - EACCES _path_ is not a regular file
* - EIO    Error reading the file
* - ENOMEM Not enough available space
* - ENOENT _path_ does not exist
* - ENOEXC Unrecognised file format
*/
int posix_spawn(pid_t *restrict pid, const char *restrict path,
                const posix_spawn_file_actions_t *file_actions,
                const posix_spawnattr_t *restrict attrp,
                char *const argv[restrict], char *const envp[restrict]);

#endif /* _SPAWN_H */
