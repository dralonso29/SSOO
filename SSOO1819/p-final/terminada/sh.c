#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <err.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <glob.h>
#include <string.h>
#include "alonsod_utils.h"

enum{
    MAX_LINE_LEN = 2048,
    MAX_TOKENS = 300,
    MAX_CMD_LEN = 4096,
    MAX_PATH = 2048,
    MAX_CONC = 1024,
    MAX_PIPES = 100,
    MAX_PARTS = 2,
    MAX_REDRS = 2,
    PERMS = 0664,
    NPENDING = 512,
    MAX_NAME = 256,
};

char delim_pipe[] = "|";
char delim_nl[] = "\n";
char delim_space[] = " \t"; // spaces and tabs
char delim_dots[] = ":";
char delim_ampersand[] = "&";
char delim_bigger[] = ">";
char delim_lower[] = "<";
char delim_dollar[] = "$";
char delim_eq[] = "=";

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
    snprintf(buffer, MAX_PATH, "./%s", cmd_splited);
    if (access(buffer, F_OK) == 0) {
        found = 1;
    }else{
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
    }
    return found;
}

void
look_redirs(int counter, int nchilds, char *in, char *out, int wait_cmd)
{
    int fdin, fdout;

    if(counter == 0) {
        if(in != NULL) {
            fdin = open(in, O_RDONLY);
        }else if (in == NULL && !wait_cmd) {
            fdin = open("/dev/null", O_RDONLY);
        }
        if (fdin < 0) {
            warn("Error while opening file\n");
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
do_fork(int nchilds, int counter, char path[], char *cmd_splited[], int *pos, int fd[][2], char *in, char *out, int wait_cmd)
{
    int pid;

    pid = fork();
    switch (pid) {
        case -1:
            err(EXIT_FAILURE, "fork");
        case 0:
            look_redirs(counter, nchilds, in, out, wait_cmd);
            if (nchilds > 1) {
                if (counter == 0) {
                    dup2(fd[counter][1], 1);
                }else if (counter == nchilds -1) {
                    dup2(fd[counter-1][0], 0);
                }else{
                    dup2(fd[counter-1][0], 0);
                    dup2(fd[counter][1], 1);
                }
                closeall(fd, nchilds -1, 0);
                closeall(fd, nchilds -1, 1);
            }
            execv(path, cmd_splited);
            err(EXIT_FAILURE, "execv");
    }
    *pos = pid;
}

void
wait_childs(int nchilds, int child_pids[])
{
    int wstatus, towait;
    pid_t pid;

    towait = nchilds;
    while(towait > 0) {
        pid = wait(&wstatus);
        if (pid < 0) {
            err(EXIT_FAILURE, "wait");
        }
        if (!is_listed(pid, child_pids, nchilds)) {
            printf("[%d] Done\n", pid);
        }else{
            towait--;
        }
    }
}

int
replace_env(char *splited[], int len)
{
    char *dollar[1];
    char *env;
    int i, exists;

    exists = 1;
    for (i = 0; i < len; i++) {
        if (splited[i][0] == '$') {
            split_by(splited[i], delim_dollar, dollar);
            env = getenv(dollar[0]);
            if (env != NULL) {
                splited[i] = env;
            }else{
                fprintf(stderr, "Error: var %s does not exist\n", splited[i]);
                exists = 0;
            }
        }
    }
    return exists;
}

int
replace_glob(char *cmd_splited[], int len)
{
    glob_t globbuf;
    char *aux[MAX_CMD_LEN];
    int pos, i;

    pos = 0;
    globbuf.gl_offs = 0;
    globbuf.gl_pathv = NULL;
    for (i = 0; i < len; i++) {
        if(glob(cmd_splited[i], GLOB_DOOFFS|GLOB_NOCHECK, NULL, &globbuf) == 0) {
            for (size_t j = 0; j < globbuf.gl_pathc; j++) {
                if (pos < MAX_CMD_LEN -1) { //need last element be NULL (execv)
                    aux[pos] = globbuf.gl_pathv[j];
                    pos++;
                }else{
                    fprintf(stderr, "Warning: Reached max values in globbing\n");
                    return -1;
                }
            }
        }
    }
    for (i = 0; i < pos; i++) {
        cmd_splited[i] = aux[i];
    }
    return 0;
}

void
create_childs(int nchilds, char *commands[], char *in, char *out, int wait_cmd)
{
    char path[MAX_PATH];
    char *cmd_splited[MAX_CMD_LEN];
    int child_pids[nchilds];
    int fd[MAX_PIPES][2];
    int npipes, i, cmd_ok, splited, replace_ok;

    cmd_ok = 0;
    replace_ok = 1;
    npipes = the_lower(MAX_PIPES, nchilds -1); //we will have at most nchilds - 1 pipes, but we put the limit on 100 pipes
    create_pipes(fd, npipes);
    for (i = 0; i < nchilds; i++) {
        set_null(cmd_splited, MAX_CMD_LEN); //set all array with null
        splited = split_by(commands[i], delim_space, cmd_splited);
        replace_ok = replace_env(cmd_splited, splited);
        if (replace_ok && replace_glob(cmd_splited, splited) == 0) {
            if (exists_path(cmd_splited[0], path)) {
                do_fork(nchilds, i, path, cmd_splited, &child_pids[i], fd, in, out, wait_cmd);
                cmd_ok = 1;
            }else{
                fprintf(stderr, "%s: command not found\n", cmd_splited[0]);
            }
        }
    }

    closeall(fd, npipes, 0);
    closeall(fd, npipes, 1);
    if (wait_cmd && cmd_ok) {
        wait_childs(nchilds, child_pids);
    }
}

void
exec_command(char *line)
{
    int nchilds, n1, n2, wait_cmd;
    char *commands[MAX_TOKENS];
    char *parts[MAX_PARTS];
    char *redirs[MAX_REDRS];
    char *in[1];
    char *out[1];

    set_null(parts, MAX_PARTS);
    set_null(redirs, MAX_REDRS);
    set_null(in, 1);
    set_null(out, 1);

    wait_cmd = appear_ampersand(line);
    split_by(line, delim_ampersand, parts);
    set_null(parts, MAX_PARTS);

    n1 = pos_on_string(line, strlen(line), '>');
    n2 = pos_on_string(line, strlen(line), '<');

    if((n1<n2 && n1 > 0 && n2 > 0) || (n1>n2 && n1 > 0 && n2 < 0)) {
        split_by(line, delim_bigger, parts);
        split_by(parts[1], delim_lower, redirs);
        split_by(redirs[1], delim_space, in);
        split_by(redirs[0], delim_space, out);
    }else{
        split_by(line, delim_lower, parts);
        if (parts[1] != NULL) {
            split_by(parts[1], delim_bigger, redirs);
            split_by(redirs[1], delim_space, out);
            split_by(redirs[0], delim_space, in);
        }
    }

    nchilds = split_by(parts[0], delim_pipe, commands);
    if(!isbuilt_in(commands[0])) {
        create_childs(nchilds, commands, in[0], out[0], wait_cmd);
    }else{
        exec_built_in(commands[0]);
    }
}

void
set_env(char *line)
{
    char *eqs[2];
    char *ws1[1];
    char *ws2[1];

    split_by(line, delim_eq, eqs);
    split_by(eqs[0], delim_space, ws1);
    split_by(eqs[1], delim_space, ws2);
    replace_env(ws2, 1);
    setenv(ws1[0], ws2[0], 1); // if exists, it will be overwritted
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
            if (strchr(line, '=') == NULL) {
                exec_command(line);
            }else{
                set_env(line);
            }
        }
    } while (line != NULL);

    printf("\n");
    exit(EXIT_SUCCESS);
}
