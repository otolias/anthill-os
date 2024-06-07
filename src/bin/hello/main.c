#include <stdio.h>

int main(void) {
    FILE *f = fopen("/bin/hello", "r");
    int c;
    unsigned i = 0;

    while ((c = fgetc(f)) != EOF) {
        i++;
    }

    printf("Read 0x%x bytes\n", i);
}
