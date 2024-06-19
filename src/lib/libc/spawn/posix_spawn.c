#include "spawn.h"

#include <errno.h>
#include <stdio.h>
#include <syscalls.h>
#include <sys/mman.h>
#include <sys/stat.h>

int posix_spawn(pid_t *restrict pid, const char *restrict path,
                __attribute__((unused)) const posix_spawn_file_actions_t *file_actions,
                __attribute__((unused)) const posix_spawnattr_t *restrict attrp,
                __attribute__((unused)) char *const argv[restrict],
                __attribute__((unused)) char *const envp[restrict]) {
    /* Get file size */
    FILE *f = fopen(path, "r");
    if (!f) { errno = ENOENT; return -1; }

    struct stat buffer;
    if (fstat(f->fid, &buffer) != 0)
        { fclose(f); errno = EACCES; return -1; }

    if (buffer.st_size == 0)
        { fclose(f); errno = EACCES; return -1; }

    /* Map file */
    char *address = mmap(0, buffer.st_size, 0, MAP_ANONYMOUS, 0, 0);
    if (address == MAP_FAILED)
        { fclose(f); errno = ENOMEM; return -1; }

    /* Load file to memory */
    size_t bytes = fread(address, 1, buffer.st_size, f);
    if (bytes != buffer.st_size) {
        munmap(address, buffer.st_size);
        fclose(f);
        errno = EIO;
        return -1;
    }

    ssize_t res = SYSCALL_2(SYS_SPAWN, (long) pid, (long) address);
    if (res <= 0) {
        errno = -res;
        munmap(address, buffer.st_size);
        fclose(f);
        return -1;
    }

    munmap(address, buffer.st_size);
    fclose(f);

    return 0;
}
