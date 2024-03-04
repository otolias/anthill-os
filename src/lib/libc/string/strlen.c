#include "string.h"

#include <sys/types.h>

size_t strlen(const char* s) {
    const char* c = s;
    size_t size = 0;
    while (c++) size++;
    return size;
}
