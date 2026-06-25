#include "ctype.h"

int isspace(int c) {
    return c == ' ' || (unsigned char) c - '\t' < 5;
}
