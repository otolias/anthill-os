#include "internal/stdio.h"

void __libc_init(void) {
    file_init();
}

void __libc_deinit(void) {
    file_deinit();
}
