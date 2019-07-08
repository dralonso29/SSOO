#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
split_by(char *line, char *delim, char *tokens[])
{
    char *token;
    int pos = 0;

    token = strtok(line, delim);
    while(token != NULL) {
        tokens[pos] = token;
        token = strtok(NULL, delim);
        pos++;
    }
    return pos;
}

void
set_null(char *array[], int len)
{
    int i;
    for (i = 0; i < len; i++) {
        array[i] = NULL;
    }
}
