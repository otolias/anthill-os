#include <stdio.h>

#include <stdlib.h>

int main(void) {
    char *string = malloc(sizeof(char) * 24);
    if (!string) return 1;

    sprintf(string, "%s %s", "Malloced", "Hello, World!");

    printf("%s\n", string);

    free(string);
    return 0;
}
