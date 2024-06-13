#include "stdio.h"

int fputs(const char *restrict s, FILE *restrict stream) {
    return fprintf(stream, "%s\n", s);
}
