#include "stdio.h"

#include "internal/stdio.h"

int vsprintf(char *str, char *format, va_list args) {
    return _formatter(str, format, args);
}
