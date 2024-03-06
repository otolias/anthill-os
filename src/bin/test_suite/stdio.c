#include "test_suite.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void _test_char(void) {
    char buffer[16];

    sprintf(buffer, "%c", 'c');
    if (strcmp(buffer, "c") != 0)
        puts("STDIO::ERROR::Character formatting failed");
}

static void _test_signed(void) {
    char buffer[64];

    sprintf(buffer, "%d", INT32_MAX);
    if (strcmp(buffer, "2147483647") != 0)
        puts("STDIO::ERROR::Positive signed integer formatting failed");

    sprintf(buffer, "%i", INT32_MIN + 1);
    if (strcmp(buffer, "-2147483647") != 0)
        puts("STDIO::ERROR::Negative signed integer formatting failed");

    sprintf(buffer, "%ld", INT64_MAX);
    if (strcmp(buffer, "9223372036854775807") != 0)
        puts("STDIO::ERROR::Positive signed long formatting failed");

    sprintf(buffer, "%ld", INT64_MIN + 1);
    if (strcmp(buffer, "-9223372036854775807") != 0)
        puts("STDIO::ERROR::Negative signed long formatting failed");
}

static void _test_unsigned(void) {
    char buffer[64];

    sprintf(buffer, "%u", UINT32_MAX);
    if (strcmp(buffer, "4294967295") != 0)
        puts("STDIO::ERROR::Unsigned decimal integer formatting failed");

    sprintf(buffer, "%x", UINT32_MAX);
    if (strcmp(buffer, "ffffffff") != 0)
        puts("STDIO::ERROR::Unsigned hexadecimal integer formatting failed");

    sprintf(buffer, "%lu", UINT64_MAX);
    if (strcmp(buffer, "18446744073709551615") != 0)
        puts("STDIO::ERROR::Unsigned decimal long formatting failed");

    sprintf(buffer, "%lx", UINT64_MAX);
    if (strcmp(buffer, "ffffffffffffffff") != 0)
        puts("STDIO::ERROR::Unsigned hexadecimal long formatting failed");
}

void test_stdio(void) {
    _test_char();
    _test_signed();
    _test_unsigned();
}
