#include "pstring.h"

size_t pstrlen(const pstring* s) {
    return sizeof(unsigned short) + s->len;
}
