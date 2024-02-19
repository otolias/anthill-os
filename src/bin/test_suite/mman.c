#include "test_suite.h"

#include <errno.h>
#include <stdio.h>
#include <sys/mman.h>

void _test_mmap(void) {
    /* --- Tests --- */

    void *res;

    errno = 0;
    void *mem = mmap(0, 512, 0, MAP_ANONYMOUS, 0, 0);
    if (mem == MAP_FAILED || errno != 0)
        puts("MMAN::ERROR::Anonymous mmap failed");

    errno = 0;
    res = mmap(0, 512, 0, 0, 0, 0);
    if (res != MAP_FAILED && errno == EACCES)
        puts("MMAN::ERROR::Argument validation failed");

    errno = 0;
    res = mmap(0, 0, 0, 0, 0, 0);
    if (res != MAP_FAILED && errno != EINVAL)
        puts("MMAN::ERROR::Invalid length value validation failed");

    /* --- Teardown --- */
    munmap(mem, 0);
}

void _test_munmap(void) {
    /* --- Setup --- */
    void *mem = mmap(0, 512, 0, MAP_ANONYMOUS, 0, 0);

    int res;

    /* --- Tests --- */

    errno = 0;
    res = munmap(mem, 0);
    if (res != 0 || errno != 0)
        puts("MMAN::ERROR::Munmap failed");

}

void test_mman(void) {
    _test_mmap();
    _test_munmap();
}
