#include "test_suite.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void _test_malloc(void) {
    /* --- Tests --- */

    errno = 0;
    void *mem = malloc(8);
    if (!mem || errno != 0)
        puts("STDLIB::ERROR::Malloc failed");

    free(mem);

    void *mem_2 = malloc(8);
    if (!mem_2 || mem != mem_2)
        puts("STDLIB::ERROR::Malloc after free assertion failed");

    free(mem_2);

    errno = 0;
    void *mem_3 = malloc(3000);
    if (!mem_3 || errno != 0)
        puts("STDLIB::ERROR::Whole block malloc failed");

    errno = 0;
    void *mem_4 = malloc(8);
    if (!mem_4 || errno != 0)
        puts("STDLIB::ERROR::New block malloc failed");

    free(mem_3);
    free(mem_4);

    errno = 0;
    void *mem_5 = malloc(5000);
    if (mem_5 != NULL && errno != ENOMEM)
        puts("STDLIB::ERROR::Huge malloc assertion failed");

    free(mem_5);
}

void test_stdlib(void) {
    _test_malloc();
}
