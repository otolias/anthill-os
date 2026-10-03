#include "test.h"

#include <kernel/panic.h>
#include <kernel/string.h>

static void _strlcpy(void) {
    char dest[8] = {0};
    const char src[] = "abcd";
    size_t res;

    res = strlcpy(dest, src, 4);
    if (res != 4 || memcmp(dest, "abcd", 5) != 0)
        panic("Strlcpy check failed");

    memset(dest, 0, 8);

    res = strlcpy(dest, src, 2);
    if (res != 4 || memcmp(dest, "ab", 3) != 0)
        panic("Strlcpy failed");

    memset(dest, 0, 8);

    res = strlcpy(dest, src, 8);
    if (res != 4 || memcmp(dest, "abcd", 5) != 0)
        panic("Strlcpy failed");
}

static void _strncmp(void) {
    char s1[] = "abcd";
    char s2[] = "abce";

    if (strncmp(s1, s2, 3) != 0)
        panic("Strncmp failed");

    if (!(strncmp(s1, s2, 4) < 0))
        panic("Strncmp failed");

    if (!(strncmp(s2, s1, 4) > 0))
        panic("Strncmp failed");
}

void _test_run_string(void) {
    _strlcpy();
    _strncmp();
}
