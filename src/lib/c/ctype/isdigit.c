#include "ctype.h"

int isdigit(int c) {
    return (unsigned char) c - '0' < 10;
}
