#include "test_suite.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void _test_formatting(void) {
    char buffer[128];
    int res;

    res = sprintf(buffer, "%c", 'c');
    if (res != 1 || strcmp(buffer, "c") != 0)
        puts("STDIO::ERROR::Character formatting failed");

    res = sprintf(buffer, "%s %s", "String 1", "String 2");
    if (res != 17 || strcmp(buffer, "String 1 String 2") != 0)
        puts("STDIO::ERROR::String formatting failed");

    res = sprintf(buffer, "%%");
    if (res != 1 || strcmp(buffer, "%") != 0)
        puts("STDIO::ERROR::Percent formatting escape failed");

    res = sprintf(buffer, "%d", INT32_MAX);
    if (res != 10 || strcmp(buffer, "2147483647") != 0)
        puts("STDIO::ERROR::Positive signed integer formatting failed");

    res = sprintf(buffer, "%i", INT32_MIN + 1);
    if (res != 11 || strcmp(buffer, "-2147483647") != 0)
        puts("STDIO::ERROR::Negative signed integer formatting failed");

    res = sprintf(buffer, "%ld", INT64_MAX);
    if (res != 19 || strcmp(buffer, "9223372036854775807") != 0)
        puts("STDIO::ERROR::Positive signed long formatting failed");

    res = sprintf(buffer, "%ld", INT64_MIN + 1);
    if (res != 20 || strcmp(buffer, "-9223372036854775807") != 0)
        puts("STDIO::ERROR::Negative signed long formatting failed");

    res = sprintf(buffer, "%u", UINT32_MAX);
    if (res != 10 || strcmp(buffer, "4294967295") != 0)
        puts("STDIO::ERROR::Unsigned decimal integer formatting failed");

    res = sprintf(buffer, "%x", UINT32_MAX);
    if (res != 8 || strcmp(buffer, "ffffffff") != 0)
        puts("STDIO::ERROR::Unsigned hexadecimal integer formatting failed");

    res = sprintf(buffer, "%lu", UINT64_MAX);
    if (res != 20 || strcmp(buffer, "18446744073709551615") != 0)
        puts("STDIO::ERROR::Unsigned decimal long formatting failed");

    res = sprintf(buffer, "%lx", UINT64_MAX);
    if (res != 16 || strcmp(buffer, "ffffffffffffffff") != 0)
        puts("STDIO::ERROR::Unsigned hexadecimal long formatting failed");
}

static void _test_snprintf(void) {
    char buffer[128];
    int res;

    res = snprintf(buffer, 128, "%s", "String");
    if (res != 6 || memcmp(buffer, "String", 7) != 0)
        puts("STDIO::ERROR::Snprintf nul termination failed");

    res = snprintf(buffer, 0, "%s", "Another String");
    if (res != 14 || memcmp(buffer, "Another String", 15) == 0)
        puts("STDIO::ERROR::Snprintf n = 0 failed");
}

static void _test_sprintf(void) {
    char buffer[128];
    int res;

    res = sprintf(buffer, "%s", "String");
    if (res != 6 || memcmp(buffer, "String", 7) != 0)
        puts("STDIO::ERROR::Sprintf nul termination failed");
}

void test_stdio(void) {
    _test_formatting();
    _test_snprintf();
    _test_sprintf();
}
