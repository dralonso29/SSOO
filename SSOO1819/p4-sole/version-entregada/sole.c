#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

enum{
        MAX_FILES = 10,
        MIN_FILES = 2,
        MAX_LEN = 50,
};

struct list{
        int pid;    //pid del hijo
        char *file_name;    //fichero asociado a un hijo
};

/*
ruta cmp(which cmp): /usr/bin/cmp
*/

int
are_names_equal(char *name1, char *name2)
{
    return !strncmp(name1, name2, MAX_LEN);
}

void
wait_child(int num_files, int *unique, int waits[])
{
    int i, wstatus;

    for(i = 0; i < num_files; i++) {
        if (waits[i]) {
            if(wait(&wstatus) < 0) {
                err(EXIT_FAILURE, "in function wait_child:wait");
            }
            if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) == 1) { //si hace exit y WEXITSTATUS es 1, el fichero es unico
                *unique = 1;
            }else if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus) != 1) {
                exit(EXIT_FAILURE); //en el momento que veamos que dos ficheros son iguales, dejamos de comprobar al resto, ya que el fichero ya no sera unico
            }
        }
    }
}

int
are_files_equal(int num_files, char * filename, char *files[])
{
    int i, pid;
    int unique = 0;
    int * waits = malloc(sizeof(int)*num_files);

    memset(waits, 0, sizeof(int)*num_files);

    for (i = 0; i < num_files; i++) {
        if (!are_names_equal(filename, files[i+1])) { //con la condicion evitamos comparar al archivo consigo mismo
            waits[i] = 1;
            pid = fork();
            switch (pid) {
                case -1:
                    err(EXIT_FAILURE, "in function compare_files:fork");
                case 0:
                    execl("/usr/bin/cmp", "cmp", "-s", filename, files[i+1], NULL);
                    err(EXIT_FAILURE, "execl");
            }
        }
    }
    wait_child(num_files, &unique, waits);
    free(waits);

    exit(!unique); //si el fichero es unico (unique = 1), hacemos salida con exito (!unique = 0)
}


void
do_cmp(int num_files, char *files[])
{
    int i, pid, wstatus;
    struct list chl_list[num_files];

    for (i = 0; i < num_files; i++) {
        pid = fork();
        switch (pid) {
            case -1:
                err(EXIT_FAILURE, "in function do_cmp:fork");
            case 0:
                are_files_equal(num_files, files[i+1], files);
            default:
                chl_list[i].pid = pid;
                chl_list[i].file_name = files[i+1];
        }
    }

    for (i = 0; i < num_files; i++) {
        if(waitpid(chl_list[i].pid, &wstatus, 0) < 0) {
            err(EXIT_FAILURE, "wait");
        }
        if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus)) {
            printf("%s no\n", chl_list[i].file_name);
        }else if(WIFEXITED(wstatus) && !WEXITSTATUS(wstatus)) {
            printf("%s yes\n", chl_list[i].file_name);
        }
    }
}

int
main(int argc, char *argv[])
{
    int num_files = argc - 1;

    if (num_files < MIN_FILES || num_files > MAX_FILES) {
        errx(EXIT_FAILURE, "Usage: %s file1 file2 ... file%d", argv[0], MAX_FILES);
    }

    do_cmp(num_files, argv);

    exit(EXIT_SUCCESS);
}
