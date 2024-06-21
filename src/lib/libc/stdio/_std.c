#include "stdio.h"

static unsigned char stdin_buf[BUFSIZ];
FILE __stdin = {
    .fid = 0,
    .flags = (1 << F_OPEN | 1 << F_READ),
    .offset = -1,
    .buf = stdin_buf,
    .r_pos = stdin_buf,
    .r_end = stdin_buf,
};

static unsigned char stdout_buf[BUFSIZ];
FILE __stdout = {
    .fid = 1,
    .flags = (1 << F_OPEN | 1 << F_WRITE | 1 << F_APPEND),
    .offset = -1,
    .buf = stdout_buf,
    .w_pos = stdout_buf,
    .w_end = stdout_buf + BUFSIZ,
};

static unsigned char stderr_buf[BUFSIZ];
FILE __stderr = {
    .fid = 2,
    .flags = (1 << F_OPEN | 1 << F_WRITE | 1 << F_APPEND),
    .offset = -1,
    .buf = stderr_buf,
    .w_pos = stderr_buf,
    .w_end = stderr_buf + BUFSIZ,
};
