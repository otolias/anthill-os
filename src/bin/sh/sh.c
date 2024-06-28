#include "sh.h"

#include <dirent.h>
#include <stddef.h>

DIR *current_dir = NULL;

enum sh_error cmd_cd(char *dir) {
    DIR *target = opendir(dir);
    if (!target) {
        fprintf(stderr, "%s not found\n", dir);
        return SH_OK;
    }

    closedir(current_dir);
    current_dir = target;
    return SH_OK;
}

enum sh_error cmd_ls(void) {
    const struct dirent *dirent;
    rewinddir(current_dir);

    while ((dirent = readdir(current_dir)))
        printf("%s ", dirent->d_name);

    fputc('\n', stdout);
    fflush(stdout);
    return SH_OK;
}

void sh_deinit(void) {
    if (current_dir) {
        closedir(current_dir);
        current_dir = NULL;
    }
}

enum sh_error sh_init(void) {
    current_dir = opendir("/");
    if (!current_dir)
        return SH_NOTFOUND;

    return SH_OK;
}
