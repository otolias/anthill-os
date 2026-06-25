#include "string.h"

#include <sys/types.h>

size_t strlen(const char* s) {
    const char* c = s;
    while (*c) c++;
    return c - s;
}
