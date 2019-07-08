#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <err.h>
#include <sys/wait.h>

#include "alonsod_utils.h"

char delim_pipe[] = "|";
char delim_nl[] = "\n";
char delim_space[] = " ";
char delim_dots[] = ":";

enum{
    MAX_LINE_LEN = 500,
    MAX_TOKENS = 300,
    MAX_CMD_LEN = 100,
    MAX_PATH = 2048,
    MAX_CONC = 1024,
};

char *
read_line()
{
    char line[MAX_LINE_LEN];

    printf("mi_sh:~$ ");
    return fgets(line, MAX_LINE_LEN, stdin);
}

int
exists_path(char *cmd_splited, char *buffer)
{
    int n, i, found;
    char *path_splited[MAX_PATH];
    char *path;

    found = 0;
    path = strndup(getenv("PATH"), MAX_PATH);
    n = split_by(path, delim_dots, path_splited);
    for (i = 0; i < n; i++) {
        snprintf(buffer, MAX_PATH,"%s/%s", path_splited[i], cmd_splited);
        if (access(buffer, F_OK) == 0) {
            found = 1;
            break;
        }
    }
    free(path);
    return found;
}

void
create_childs(int nchilds, char *commands[])
{
    char path[MAX_PATH];
    char *cmd_splited[MAX_CMD_LEN];
    int n, pid;

    for (size_t i = 0; i < nchilds; i++) {
        set_null(cmd_splited, MAX_CMD_LEN); //set all array with null
        split_by(commands[i], delim_space, cmd_splited);
        if (exists_path(cmd_splited[0], path)) {
            pid = fork();
            switch (pid) {
                case -1:
                    err(EXIT_FAILURE, "fork");
                case 0:
                    execv(path, cmd_splited);
                    err(EXIT_FAILURE, "execv");
            }
        }else{
            fprintf(stderr, "%s: command not found\n", cmd_splited[0]);
        }
    }

    for (size_t k = 0; k < nchilds; k++) {
        wait(NULL);
    }
}

void
exec_command(char *line)
{
    int nchilds;
    char *commands[MAX_TOKENS];

    nchilds = split_by(line, delim_pipe, commands);
    // printf("%d\n%s\n",nchilds, commands[0]);
    if(!isbuilt_in(commands[0])) {
        create_childs(nchilds, commands);
    }else{
        exec_built_in(commands[0]);
    }
}

int
main(int argc, char *argv[])
{
    char *line;
    char *commands[MAX_TOKENS];

    do {
        line = read_line();
        if (line != NULL) {
            split_by(line, delim_nl, commands);
            exec_command(line);
        }
    } while (line != NULL);

    printf("\n");
    exit(EXIT_SUCCESS);
}
