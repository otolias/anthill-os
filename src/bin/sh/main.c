#include <errno.h>
#include <spawn.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sh.h"

static enum sh_error _loop(char **buf, size_t *buf_size) {
    size_t pos = 0;
    int c = 0;

    while(c != '\n') {
        if (pos == *buf_size) {
            size_t new_size = *buf_size ? *buf_size * 2 : 64;
            char *tmp = realloc(*buf, new_size);
            if (!tmp) return SH_NOMEM;
            *buf = tmp;
            *buf_size = new_size;
        }

        c = fgetc(stdin);

        switch (c) {
            case '\003': /* Control-C */
                (*buf)[pos++] = 0;
                fputc('\n', stdout);
                fflush(stdout);
                return SH_CONT;
            case EOF:
                continue;
            case '\n':
                (*buf)[pos++] = 0;
                fputc(c, stdout);
                fflush(stdout);
                return SH_OK;
            default:
                (*buf)[pos++] = c;
                fputc(c, stdout);
                fflush(stdout);
                break;
        }
    }

    return SH_OK;
}

static enum sh_error _execute(char *buf) {
    int argc = strcnt(buf, ' ') + 1;
    char *argv[argc + 1];
    char *cmd = strtok(buf, " ");
    argv[0] = cmd;
    int i;

    for (i = 1; i < argc; i++) {
        argv[i] = strtok(NULL, " ");
    }

    argv[i] = NULL;

    if (strcmp(cmd, "cd") == 0)
        return cmd_cd(argc, argv);
    else if (strcmp(cmd, "importfs") == 0)
        return cmd_importfs(argc, argv);
    else if (strcmp(cmd, "ls") == 0)
        return cmd_ls();
    else {
        errno = 0;
        pid_t pid;

        if (posix_spawn(&pid, cmd, 0, 0, argv, 0) != 0) {
            switch (errno) {
                case EACCES:
                case ENOEXEC:
                    return SH_INVALID;
                case EIO:
                case ENOENT:
                    return SH_NOTFOUND;
                case ENOMEM:
                    return SH_NOMEM;
            }
        }
    }

    return SH_OK;
}

int main(void) {
    char *buf = NULL;
    size_t buf_size = 0;

    if (sh_init() != SH_OK) {
        sh_deinit();
        return 1;
    }

    while (1) {
        printf("> ");
        fflush(stdout);
        bool exit = false;

        switch (_loop(&buf, &buf_size)) {
            case SH_NOMEM:
                fprintf(stderr, "No memory\n");
                exit = true;
                break;
            case SH_CONT:
                continue;
            case SH_INVALID:
            case SH_NOTFOUND:
            case SH_OK:
                break;
        }

        switch (_execute(buf)) {
            case SH_INVALID:
                fprintf(stderr, "%s is not an executable\n", buf);
                fflush(stderr);
                break;
            case SH_NOTFOUND:
                fprintf(stderr, "%s not found\n", buf);
                fflush(stderr);
                break;
            case SH_NOMEM:
                fprintf(stderr, "No memory\n");
                exit = true;
                break;
            case SH_CONT:
            case SH_OK:
                break;
        }

        if (exit) {
            fflush(stderr);
            break;
        }
    }

    if (buf)
        { free(buf); buf = NULL; }
}
