#include "kernel/panic.h"

#include <kernel/io.h>

void _panic(const char *msg, const char *file, int line, const char *func) {
    io_fmt("\n\n----- KERNEL PANIC -----\n");
    io_fmt("%s::%s::%d::%s\n", file, func, line, msg);
    io_fmt("Hanging...\n");

    for (;;)
        ;
}
