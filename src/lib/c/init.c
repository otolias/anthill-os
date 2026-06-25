#include "internal/stdio.h"
#include "internal/stdlib.h"

void __libc_init(void) {
    file_init();
}

void __libc_deinit(void) {
    file_deinit();
    block_deinit();
}
