#include <stdio.h>

#include "test_suite.h"

int main(void) {
    puts("Starting userspace tests...");

    test_fcall();
    test_mqueue();
    test_mman();
    test_stdio();
    test_stdlib();
    test_string();
    test_unistd();

    puts("Tests done");
}
