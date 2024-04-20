#include "test_suite.h"

#include <stdio.h>
#include <unistd.h>

static void _test_getpid(void) {
    if (getpid() < 0)
        puts("UNISTD::ERROR::getpid failed");
}

void test_unistd(void) {
    _test_getpid();
}
