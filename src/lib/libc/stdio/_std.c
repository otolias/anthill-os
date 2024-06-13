#include "stdio.h"

static char stdin_buf[BUFSIZ];
FILE __stdin = {
    .fid = 0,
    .flags = (1 << F_OPEN),
    .buf = stdin_buf,
    .buf_pos = stdin_buf,
    .buf_end = stdin_buf + BUFSIZ,
};

static char stdout_buf[BUFSIZ];
FILE __stdout = {
    .fid = 1,
    .flags = (1 << F_OPEN),
    .buf = stdout_buf,
    .buf_pos = stdout_buf,
    .buf_end = stdout_buf + BUFSIZ,
};

static char stderr_buf[BUFSIZ];
FILE __stderr = {
    .fid = 2,
    .flags = (1 << F_OPEN),
    .buf = stderr_buf,
    .buf_pos = stderr_buf,
    .buf_end = stderr_buf + BUFSIZ,
};
