#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

enum{
        MAX_SIZE_TOKENS = 100,
};

int
tokenize_var(char * content, char * delim, char * tokens[])
{
    char * token;
    int pos = 0;

    token = strtok(content, delim);
    while(token != NULL) {
        tokens[pos] = token;
        token = strtok(NULL, delim);
        pos++;
    }
    return pos;
}

int
main(int argc, char *argv[])
{
    int total;
    char * tokens[MAX_SIZE_TOKENS];
    char delim[] = ":";
    char content[] = "hola:casa:pepe";

    total = tokenize_var(content, delim, tokens);
    printf("%d\n", total);
    for (int i = 0; i < total; i++) {
        printf("Pos %d -> %s\n",i, tokens[i]);
    }
    exit(EXIT_SUCCESS);
}
