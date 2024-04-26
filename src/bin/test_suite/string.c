#include "test_suite.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

static void _test_memmove(void) {
    char *p;

    {
        char arr[] = {'a', 'b', 'c'};
        p = arr;
        memmove(p, p + 1, 2);
        if (p[0] != 'b' || p[1] != 'c')
            puts("STRING::ERROR::memmove s > d failed");
    }

    {
        char arr[] = {'a', 'b', 'c'};
        p = arr;
        memmove(p + 1, p, 2);
        if (p[1] != 'a' || p[2] != 'b')
            puts("STRING::ERROR::memmove s < b failed");
    }

    {
        char arr[] = {'a', 'b', 'c'};
        p = arr;
        memmove(p, p, 2);
        if (p[0] != 'a' || p[1] != 'b')
            puts("STRING::ERROR::memmove s == b failed");
    }
}

static void _test_strchr(void) {
    const char s[] = "ab";

    if (strchr(s, 'b') == NULL)
        puts("STRING::ERROR::strchr failed");

    if (strchr(s, 'c') != NULL)
        puts("STRING::ERROR::strchr NULL check failed");

    if (strchr("", 'a') != NULL)
        puts("STRING::ERROR::strchr empty string check failed");
}

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
    _test_memmove();
    _test_strchr();
    _test_strspn();
    _test_strcspn();
    _test_strtok();
}
