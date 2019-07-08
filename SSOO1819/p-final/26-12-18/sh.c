#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "alonsod_utils.h"

char delim_pipe[] = "|";
char delim_nl[] = "\n";
char delim_space[] = " ";
char delim_dots[] = ":";
char delim_ampersand[] = "&";
char delim_bigger[] = ">";
char delim_lower[] = "<";

enum{
    MAX_LINE_LEN = 500,
    MAX_TOKENS = 300,
    MAX_CMD_LEN = 100,
    MAX_PATH = 2048,
    MAX_CONC = 1024,
    MAX_PIPES = 100,
    MAX_PARTS = 2,
    MAX_REDRS = 2,
    PERMS = 0664,
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
look_redirs(int counter, int nchilds, char *in, char *out)
{
    int fdin, fdout;

    printf("in:%s\n", in);
    printf("out:%s\n", out);
    if (counter == 0 && in != NULL) {
        fdin = open(in, O_RDONLY);
        if (fdin < 0) {
            warn("Error while opening file %s\n", in);
        }
        dup2(fdin, 0);
        close(fdin);
    }
    if (counter == nchilds -1 && out != NULL) {
        fdout = open(out, O_WRONLY|O_CREAT|O_TRUNC, PERMS);
        if (fdout < 0) {
            warn("Error while opening file %s\n", out);
        }
        dup2(fdout, 1);
        close(fdout);
    }
}

void
do_fork(int nchilds, int counter, char path[], char *cmd_splited[], int *pos, int fd[][2], char *in, char *out)
{
    int pid;

    pid = fork();
    switch (pid) {
        case -1:
            err(EXIT_FAILURE, "fork");
        case 0:
            look_redirs(counter, nchilds, in, out);
            if (nchilds > 1) {
                if (counter == 0) {
                    printf("Soy el primer comando: %s\n", cmd_splited[0]);
                    dup2(fd[counter][1], 1);
                }else if (counter == nchilds -1) {
                    printf("Soy el ultimo comando: %s\n", cmd_splited[0]);
                    dup2(fd[counter-1][0], 0);
                }else{
                    printf("Soy un comando intermedio(numero %d): %s\n",counter, cmd_splited[0]);
                    dup2(fd[counter-1][0], 0);
                    dup2(fd[counter][1], 1);
                }
                closeall(fd, MAX_PIPES, 0); // si solo se hiciesen nchilds -1 pipes, tan solo cerrariamos aquellos que hemos usado. De la forma quye esta hecho lo que ocurre es que tenemos que cerrar todos lo s pipes aunque no los hallamos usado.
                closeall(fd, MAX_PIPES, 1);
            }
            // printf("El comando %s tiene el pid %d\n",cmd_splited[0], getpid());

            execv(path, cmd_splited);
            err(EXIT_FAILURE, "execv");
    }
    *pos = pid;
}

void
create_childs(int nchilds, char *commands[], char *in, char *out)
{
    char path[MAX_PATH];
    char *cmd_splited[MAX_CMD_LEN];
    int n;
    int child_pids[nchilds];
    int fd[MAX_PIPES][2]; //we will have at most nchilds - 1 pipes, but we put the limit on 100 pipes

    create_pipes(fd, MAX_PIPES); //can improve this function looking if MAX_PIPES is bigger or not than nchilds - 1
    for (size_t i = 0; i < nchilds; i++) {
        set_null(cmd_splited, MAX_CMD_LEN); //set all array with null
        split_by(commands[i], delim_space, cmd_splited);
        if (exists_path(cmd_splited[0], path)) {
            do_fork(nchilds, i, path, cmd_splited, &child_pids[i], fd, in, out);
            // child_pids[i] = pid;
        }else{
            fprintf(stderr, "%s: command not found\n", cmd_splited[0]);
        }
    }

    closeall(fd, MAX_PIPES, 0);
    closeall(fd, MAX_PIPES, 1);
    for (size_t k = 0; k < nchilds; k++) {
        printf("Termino el hijo con el pid %d\n", child_pids[k]);
        waitpid(child_pids[k], NULL, 0);
    }
}

void
exec_command(char *line)
{
    int nchilds, n1, n2;//, wait_cmd;
    char *commands[MAX_TOKENS];
    char *parts[MAX_PARTS];
    char *redirs[MAX_REDRS];
    char *in;
    char *out;

    set_null(parts, MAX_PARTS);
    set_null(redirs, MAX_REDRS);
    // char *amp;

    // wait_cmd = 1;
    // amp = strrchr(line, '&');
    // if (amp != NULL && strlen(amp) == 1) { // line ending on &
    //     wait_cmd = 0;
    //     //split_by(line, delim_pipe, commands);
    // }else{
    //     fprintf(stderr, "Error: ampersand must be last string\n");
    // }
    // printf("wait_cmd = %d\n", wait_cmd);

    n1 = pos_on_string(line, strlen(line), '>');
    n2 = pos_on_string(line, strlen(line), '<');

    if((n1<n2 && n1 > 0 && n2 > 0) || (n1>n2 && n1 > 0 && n2 < 0)) {
        split_by(line, delim_bigger, parts);
        split_by(parts[1], delim_lower, redirs);
        // delete_spaces(redirs[1], in);
        // delete_spaces(redirs[0], out);
        /*


        falta quitar los espacios de los ficheros de entrada y salida.


        */
        in = redirs[1];
        out = redirs[0];
    }else{
        split_by(line, delim_lower, parts);
        if (parts[1] != NULL) {
            split_by(parts[1], delim_bigger, redirs);
            // delete_spaces(redirs[1], out);
            // delete_spaces(redirs[0], in);
        }
        out = redirs[1];
        in = redirs[0];
    }
    // printf("%s\n", parts[0]);
    printf("in:-%s-\n", in);
    printf("out:-%s-\n", out);
    // if (parts[1] != NULL) {
    //
    // }

    nchilds = split_by(parts[0], delim_pipe, commands);
    if(!isbuilt_in(commands[0])) {
        create_childs(nchilds, commands, in, out);
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
        if (line != NULL && strlen(line)>1) {
            split_by(line, delim_nl, commands);
            exec_command(line);
            // printf("El caracter %c esta en la posicion %d del string %s\n",'>', pos_on_string(line, strlen(line), '>'), line );
        }

    } while (line != NULL);

    printf("\n");
    exit(EXIT_SUCCESS);
}
