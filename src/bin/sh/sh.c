#include "sh.h"

#include <dirent.h>
#include <stddef.h>
#include <unistd.h>

DIR *current_dir = NULL;

enum sh_error cmd_cd(int argc, char *argv[]) {
    if (argc == 1) {
        fputs("No directory given\n", stderr);
        fflush(stderr);
        return SH_OK;
    }

    DIR *target = opendir(argv[1]);
    if (!target) {
        fprintf(stderr, "%s not found\n", argv[1]);
        fflush(stderr);
        return SH_OK;
    }

    closedir(current_dir);
    current_dir = target;
    return SH_OK;
}

enum sh_error cmd_importfs(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "%s: Usage: importfs _channel_ _path_\n", argv[0]);
        return SH_OK;
    }

    FILE *channel = fopen(argv[1], "r+");
    if (!channel) {
        fprintf(stderr, "%s: Failed to open %s\n", argv[0], argv[1]);
        return SH_OK;
    }

    if (mount(fileno(channel), argv[2]) == -1) {
        fprintf(stderr, "%s: Failed to mount %s\n", argv[0], argv[2]);
        return SH_OK;
    }

    puts("Connection Initialised");

    fclose(channel);
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
