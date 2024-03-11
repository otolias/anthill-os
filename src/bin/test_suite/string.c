#include "test_suite.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

static void _test_strspn(void) {
    char s[] = "str/str-";

    if (strspn(s, "") != 0)
        puts("STRING::ERROR::strspn with empty string failed");

    if (strspn(s, "str/") != 7)
        puts("STRING::ERROR::strspn failed");
}

static void _test_strcspn(void) {
    const char s[] = "str/str-";

    if (strcspn(s, "") != 8)
        puts("STRING::ERROR::strcspn with empty string failed");

    if (strcspn(s, "/") != 3)
        puts("STRING::ERROR::strcspn with byte in the middle failed");

    if (strcspn(s, "-") != 7)
        puts("STRING::ERROR::strcspn with byte at the end failed");

    if (strcspn(s, "-/") != 3)
        puts("STRING::ERROR::strcspn with multiple bytes failed");
}

static void _test_strtok(void) {
    char string[] = "A split ,string";
    const char *token;

    token = strtok(string, " ,");
    if (strcmp(token, "A") != 0)
        puts("STRING::ERROR::strtok failed");

    token = strtok(NULL, " ,");
    if (strcmp(token, "split") != 0)
        puts("STRING::ERROR::strtok failed");

    token = strtok(NULL, " ,");
    if (strcmp(token, "string") != 0)
        puts("STRING::ERROR::strtok failed");

    token = strtok(NULL, " ,");
    if (token != NULL)
        puts("STRING::ERROR::strtok failed");
}

void test_string(void) {
    _test_strspn();
    _test_strcspn();
    _test_strtok();
}
