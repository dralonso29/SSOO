#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

char MAX_PACK[] = "1";
char MAX_SEC[] = "5";

enum{
        MAX_IPS = 5,
};

void
wait_child(int num_IP, int *flag)
{
    int i, wstatus;

    for(i = 0; i < num_IP; i++) {
        if(wait(&wstatus) < 0) {
            err(EXIT_FAILURE, "wait");
        }

        if (WIFEXITED(wstatus) && WEXITSTATUS(wstatus)) { //si termina con exit, pero su salida es distinta de cero, terminamos con status de error
            *flag = EXIT_FAILURE;
        }
    }
}

int
do_ping(int num_IP, char *IP[])
{
    int i, pid;
    int terminar = EXIT_SUCCESS;

    for(i = 0; i < num_IP; i++) {
        pid = fork();
        switch(pid) {
            case -1:
                err(EXIT_FAILURE, "fork");
            case 0:
                execl("/bin/ping", "ping", "-c", MAX_PACK, "-W", MAX_SEC, IP[i+1], NULL); //hacemos i+1 para que no lea el nombre del programa
                err(1, "execl");
        }
    }

    wait_child(num_IP, &terminar); //espera a los hijos y si hay fallo, cambia el valor de la variable terminar

    return terminar;
}

int
main(int argc, char *argv[])
{
    int num_IP, new_exit;

    if (argc == 1 || argc - 1 > MAX_IPS) {
        errx(1, "Usage: %s IP1 IP2 ... IP%d",argv[0], MAX_IPS);
    }

    num_IP = argc - 1;
    new_exit = do_ping(num_IP, argv);

    exit(new_exit);
}
