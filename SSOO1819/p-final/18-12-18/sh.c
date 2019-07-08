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

enum{
    MAX_LINE_LEN = 500,
    MAX_TOKENS = 300,
    MAX_CMD_LEN = 100,
};

char *
read_line()
{
    char line[MAX_LINE_LEN];

    printf("mi_sh:~$ ");
    return fgets(line, MAX_LINE_LEN, stdin);
}

void
create_childs(int nchilds, char *commands[])
{
    // char *cmd_path;
    char *cmd_splited[MAX_CMD_LEN];
    int n, pid;

    for (size_t i = 0; i < nchilds; i++) {
        set_null(cmd_splited, MAX_CMD_LEN); //set all array with null
        n = split_by(commands[i], delim_space, cmd_splited);
        // for (size_t j = 0; j < MAX_CMD_LEN; j++) {
        //     if (cmd_splited[j] != NULL) {
        //         printf("XXXX %s\n", cmd_splited[j]);
        //     }
        // }
        // printf("n: %d\n", n);
        // printf("%s\n",cmd_splited[0]);
        pid = fork();
        switch (pid) {
            case -1:
                err(EXIT_FAILURE, "fork");
            case 0:
                execv("/bin/ls", cmd_splited); //seguir con el getpath que de momento solo funciona con el ls
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
    // struct command command1[];

    nchilds = split_by(line, delim_pipe, commands);
    printf("%d\n", nchilds);
    create_childs(nchilds, commands);
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
