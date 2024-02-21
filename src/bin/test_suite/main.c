#include <stdio.h>

#include "test_suite.h"

int main(void) {
    puts("Starting userspace tests...");

    test_mqueue();
    test_mman();
    test_stdlib();

    puts("Tests done");
}
