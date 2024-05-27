#include "test_suite.h"

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void _test_calloc(void) {
    int *mem;

    errno = 0;
    mem = calloc(2, sizeof *mem);
    mem[0] = (int) ~0;
    mem[1] = (int) ~0;
    free(mem); mem = NULL;

    errno = 0;
    mem = calloc(2, sizeof *mem);
    if (!mem || mem[0] != 0 || mem[1] != 0 || errno != 0)
        puts("STDLIB::ERROR::Calloc failed");

    free(mem); mem = NULL;

    errno = 0;
    mem = calloc(0, sizeof *mem);
    if (mem || errno != EINVAL) {
        puts("STDLIB::ERROR::Calloc zeroed nelem assertion failed");
        free(mem); mem = NULL;
    }

    errno = 0;
    mem = calloc(2, 0);
    if (mem || errno != EINVAL) {
        puts("STDLIB::ERROR::Calloc zeroed elsize assertion failed");
        free(mem); mem = NULL;
    }
}

static void _test_malloc(void) {
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

static void _test_realloc(void) {
    {
        char *src = malloc(2);
        src[0] = 'a'; src[1] = 'b';
        errno = 0;
        char *dst = realloc(src, 4);
        if (dst == NULL || errno != 0 || memcmp(dst, "ab", 2) != 0)
            puts("STDLIB::ERROR::Realloc bigger allocation failed");

        if (dst) free(dst);
        else free(src);
    }

    {
        char *src = malloc(4);
        src[0] = 'a'; src[1] = 'b'; src[2] = 'c'; src[3] = 'd';
        errno = 0;
        char *dst = realloc(src, 2);
        if (dst == NULL || errno != 0 || memcmp(dst, "ab", 2) != 0)
            puts("STDLIB::Error::Realloc smaller allocation failed");

        if (dst) free(dst);
        else free(src);
    }

    {
        char *src = malloc(2);
        errno = 0;
        char *dst = realloc(src, 0);
        if (dst != NULL || errno != EINVAL)
            puts("STDLIB::ERROR::Realloc size check failed");

        if (dst) free(dst);
        else free(src);
    }

    {
        errno = 0;
        char *dst = realloc(NULL, 4);
        if (dst == NULL || errno != 0)
            puts("STDLIB::ERROR::Realloc null src failed");

        if (dst) free(dst);
    }
}

static void _test_strtoull(void) {
    char *end;
    unsigned long long res;

    errno = 0;
    res = strtoull("123", NULL, 0);
    if (res != 123 || errno != 0)
        puts("STDLIB::ERROR::strtoull failed");

    errno = 0;
    res = strtoull(" \f\n\r\t\v123", NULL, 0);
    if (res != 123 || errno != 0)
        puts("STDLIB::ERROR::strtoull with whitespace failed");

    errno = 0;
    res = strtoull("123 rest", &end, 0);
    if (res != 123 || errno != 0 || strcmp(end, " rest") != 0)
        puts("STDLIB::ERROR::strtoull end pointer failed");

    errno = 0;
    res = strtoull("123", NULL, 6);
    if (res != 0 || errno != EINVAL)
        puts("STDLIB::ERROR::strtoull base check failed");

    errno = 0;
    res = strtoull("+123", NULL, 10);
    if (res != 123 && errno != 0)
        puts("STDLIB::ERROR::strtoull plus check failed");

    errno = 0;
    res = strtoull("-123", NULL, 10);
    if (res != 18446744073709551493ULL || errno != 0)
        puts("STDLIB::ERROR::strtoull minus check failed");

    errno = 0;
    res = strtoull("18446744073709551616", NULL, 10); /* ULLONG_MAX + 1ULL */
    if (res != 0 || errno != ERANGE)
        puts("STDLIB::ERROR::strtoull limit check failed");
}

static void _test_strtoul(void) {
    char *end;
    unsigned long res;

    errno = 0;
    res = strtoul("123", NULL, 0);
    if (res != 123 || errno != 0)
        puts("STDLIB::ERROR::strtoul failed");

    errno = 0;
    res = strtoul("123 rest", &end, 0);
    if (res != 123 || errno != 0 || strcmp(end, " rest") != 0)
        puts("STDLIB::ERROR::strtoul end pointer failed");

    errno = 0;
    res = strtoull("18446744073709551616", NULL, 10); /* ULONG_MAX + 1ULL */
    if (res != 0 || errno != ERANGE)
        puts("STDLIB::ERROR::strtoul limit check failed");

}

static void _test_strtoll(void) {
    char *end;
    long long res;

    errno = 0;
    res = strtoll("123", NULL, 0);
    if (res != 123 || errno != 0)
        puts("STDLIB::ERROR::strtoll failed");

    errno = 0;
    res = strtoll("123 rest", &end, 0);
    if (res != 123 || errno != 0 || strcmp(end, " rest") != 0)
        puts("STDLIB::ERROR::strtoll end pointer failed");

    errno = 0;
    res = strtoll("-123", NULL, 0);
    if (res != -123 || errno != 0)
        puts("STDLIB::ERROR::strtoll minus check failed");

    errno = 0;
    res = strtoll("9223372036854775808", NULL, 10); /* LLONG_MAX + 1LL */
    if (res != 0 || errno != ERANGE)
        puts("STDLIB::ERROR::strtoll upper limit check failed");

    errno = 0;
    res = strtoll("-9223372036854775809", NULL, 10); /* LLONG_MIN - 1LL */
    if (res != 0 || errno != ERANGE)
        puts("STDLIB::ERROR::strtoll lower limit check failed");
}

static void _test_strtol(void) {
    char *end;
    long res;

    errno = 0;
    res = strtol("123", NULL, 0);
    if (res != 123 || errno != 0)
        puts("STDLIB::ERROR::strtol failed");

    errno = 0;
    res = strtol("123 rest", &end, 0);
    if (res != 123 || errno != 0 || strcmp(end, " rest") != 0)
        puts("STDLIB::ERROR::strtol end pointer failed");

    errno = 0;
    res = strtol("-123", NULL, 0);
    if (res != -123 || errno != 0)
        puts("STDLIB::ERROR::strtol minus check failed");

    errno = 0;
    res = strtoll("9223372036854775808", NULL, 10); /* LONG_MAX + 1LL */
    if (res != 0 || errno != ERANGE)
        puts("STDLIB::ERROR::strtol limit check failed");

    errno = 0;
    res = strtoll("-9223372036854775809", NULL, 10); /* LONG_MIN - 1LL */
    if (res != 0 || errno != ERANGE)
        puts("STDLIB::ERROR::strtol lower limit check failed");
}

void test_stdlib(void) {
    _test_calloc();
    _test_malloc();
    _test_realloc();
    _test_strtoull();
    _test_strtoul();
    _test_strtoll();
    _test_strtol();
}
