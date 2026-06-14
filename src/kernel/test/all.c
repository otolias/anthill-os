#include "test.h"

#include <kernel/io.h>

void test_run_all(void) {
    io_fmt("Running memory tests... ");
    _test_run_mem();
    io_fmt("Done\n");
}
