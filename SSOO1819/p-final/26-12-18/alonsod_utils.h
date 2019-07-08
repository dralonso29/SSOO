#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum {
    MAX_STRLEN = 256,
    LEN_SPLITED = 20,
};

int
split_by(char *line, char *delim, char *tokens[])
{
    char *token;
    int pos;

    pos=0;
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

int
isbuilt_in(char *command)
{
    char *cmd_splited[LEN_SPLITED];
    char *path;
    int flag = 0;

    path = strndup(command, MAX_STRLEN);
    split_by(path, " ", cmd_splited);
    if (strncmp(cmd_splited[0],"cd" ,MAX_STRLEN) == 0) {
        flag = 1;
    }
    free(path);
    return flag;
}

void
exec_built_in(char *commands) {
    int n;
    char *cmd_splited[LEN_SPLITED];

    n = split_by(commands, " ", cmd_splited);
    if (n == 1) {
        chdir(getenv("HOME"));
    }else{
        chdir(cmd_splited[1]);
    }
}

void
create_pipes(int fd[][2], int numpipes)
{
    int i;
    for(i = 0; i < numpipes; i++) {
        if(pipe(fd[i]) < 0) {
            err(EXIT_FAILURE, "pipe");
        }
    }
}

void
closeall(int fd[][2], int n, int extr)
{
    int i;
    for(i = 0; i < n; i++) {
        close(fd[i][extr]);
    }
}

int
pos_on_string(char *str, int len, char c)
{
    int pos, i;

    // printf("len:%d\n", len);
    pos = 0;
    for (i = 0; i < len; i++) {
        // printf("%c\n", str[i]);
        if (str[i] == c) {
            break;
        }
        pos++;
    }
    if (pos == len) {
        pos = -1;
    }
    // printf("pos = %d\n", pos);
    return pos;
}

// void
// delete_spaces(char *string, char *result)
// {
//     char *splited[MAX_SPLITED];
//
//     split_by(string, " ", splited);
//     printf("-%s-\n",splited[0] );
//     *result = splited[0];
// }
