#include "test_suite.h"

#include <pstring.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void _test_pstrconv(void) {
    char buf[16];
    pstring *pstr;

    pstr = pstrconv(buf, "a", 16);
    if (pstr != (void *) buf || pstr->len != 1 || memcmp(pstr->s, "a", 1) != 0)
        puts("PSTRING::ERROR::pstrconv failed");

    pstr = pstrconv(NULL, "a", 0);
    if (pstr == NULL || pstr->len != 1 || memcmp(pstr->s, "a", 1) != 0)
        puts("PSTRING::ERROR::pstrconv dest = NULL failed");
    else
        free(pstr);

    pstr = pstrconv(buf, "ab", 1);
    if (pstr != NULL)
        puts("PSTRING::ERROR::pstrconv length check failed");
}

static void _test_pstrdup(void) {
    char src_buf[16];
    char dst_buf[16];
    const pstring *src = pstrconv(src_buf, "abc", 16);
    pstring *pstr;

    pstr = pstrdup(dst_buf, src, 16);
    if (pstr != (void *) dst_buf || pstr->len != 3 || memcmp(pstr->s, "abc", 3) != 0)
        puts("PSTRING::ERROR::pstrdup failed");

    pstr = pstrdup(NULL, src, 0);
    if (pstr == NULL || pstr->len != 3 || memcmp(pstr->s, "abc", 3) != 0)
        puts("PSTRING::ERROR::pstrdup dest = NULL failed");
    else
        free(pstr);

    pstr = pstrdup(dst_buf, src, 1);
    if (pstr != NULL)
        puts("PSTRING::ERROR::pstrdup length check failed");
}

static void _test_pstrtoz(void) {
    char buf[16];
    pstring *pstr = pstrconv(NULL, "abc", 0);
    char *res;

    res = pstrtoz(buf, pstr, 16);
    if (res != (void *) buf || memcmp(res, "abc", 4) != 0)
        puts("PSTRING::ERROR::pstrtoz failed");

    res = pstrtoz(NULL, pstr, 0);
    if (res == NULL || memcmp(res, "abc", 4) != 0)
        puts("PSTRING::ERROR::pstrtoz dest = NULL failed");
    else
        free(res);

    res = pstrtoz(buf, pstr, 1);
    if (res != NULL)
        puts("PSTRING::ERROR::pstrtoz length check failed");

    free(pstr);
}

void test_pstring(void) {
    _test_pstrconv();
    _test_pstrdup();
    _test_pstrtoz();
}
