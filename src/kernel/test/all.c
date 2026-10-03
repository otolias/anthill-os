#include "test.h"

#include <kernel/io.h>

void test_run_all(void) {
    io_fmt("Running memory tests... ");
    _test_run_mem();
    io_fmt("Done\n");

    io_fmt("Running message buffer tests... ");
    _test_run_msgbuf();
    io_fmt("Done\n");

    io_fmt("Running string tests... ");
    _test_run_string();
    io_fmt("Done\n");
}
