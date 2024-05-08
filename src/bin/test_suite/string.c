#include "test_suite.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

static void _test_memcmp(void) {
    const char *abc = "abc";
    const char *abc_2 = "abc";
    const char *abd = "abd";
    const char *bcd = "bcd";

    if (memcmp(abc, abc_2, 4) != 0)
        puts("STRING::ERROR::memcmp failed");

    if (memcmp(abc, abd, 2) != 0)
        puts("STRING::ERROR::memcmp tail failed");

    if (memcmp(abc, bcd, 2) != -1)
        puts("STRING::ERROR::memcmp negative result failed");

    if (memcmp(bcd, abc, 2) != 1)
        puts("STRING::ERROR::memcmp positive result failed");
}

static void _test_memcpy(void) {
    char p[4] = { 0 };
    char *res;

    res = memcpy(p, "12", 0);
    if (res != p || p[0] != 0 || p[1] != 0)
        puts("STRING::ERROR::memcpy n = 0 failed");

    res = memcpy(p, "12", 2);
    if (res != p || p[0] != '1' || p[1] != '2')
        puts("STRING::ERROR::memcpy char * failed");

    int i = 123;
    res = memcpy(p, &i, 4);
    if (res != p || *p != i)
        puts("STRING::ERROR::memcpy int * failed");
}

static void _test_memmove(void) {
    char p[4];

    memcpy(p, "abc", 4);
    memmove(p, p + 1, 2);
    if (memcmp(p, "bcc", 3) != 0)
        puts("STRING::ERROR::memmove s > d failed");

    memcpy(p, "abc", 4);
    memmove(p + 1, p, 2);
    if (memcmp(p, "aab", 3) != 0)
        puts("STRING::ERROR::memmove s < d failed");

    memcpy(p, "abc", 4);
    memmove(p, p, 2);
    if (memcmp(p, "abc", 3) != 0)
        puts("STRING::ERROR::memmove s == d failed");
}

static void _test_memset(void) {
    char s[] = "abcdef";
    memset(s + 1, 'z', 3);
    if (strcmp("azzzef", s) != 0)
        puts("STRING::ERROR::memset failed");
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

static void _test_strcnt(void) {
    char s[] = "aba";

    if (strcnt(s, 'b') != 1)
        puts("STRING::ERROR::strcnt occurence failed");

    if (strcnt(s, 'a') != 2)
        puts("STRING::ERROR::strcnt occurences failed");

    if (strcnt("", 'a') != 0)
        puts("STRING::ERROR::strcnt with empty string failed");

    if (strcnt("", 0) != 0)
        puts("STRING::ERROR::strcnt with null character failed");

    if (strcnt(s, 'c') != 0)
        puts("STRING::ERROR::strcnt with non existing character failed");
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
    _test_memcmp();
    _test_memcpy();
    _test_memmove();
    _test_memset();
    _test_strchr();
    _test_strcnt();
    _test_strspn();
    _test_strcspn();
    _test_strtok();
}
