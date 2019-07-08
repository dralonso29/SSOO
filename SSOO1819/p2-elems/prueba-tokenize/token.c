#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

void
tokenize(char * str, char * delim)
{
    char * token = strtok(str, delim);
    if (token != NULL) {
        printf("%s\n", token);
    }
}

int
main(int argc, char *argv[]) {
    char * delim = ":";

    for (size_t i = 1; i < argc; i++) {
        tokenize(argv[i], delim);
    }


    exit(EXIT_SUCCESS);
}
