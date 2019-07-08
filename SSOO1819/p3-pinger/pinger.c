#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

enum{
        MAX_IPS = 10,
        MAX_PACKETS = 1,
        TIMEWAIT = 5,
};

void
do_pings(char * dir_IP[], int num_IP)
{
    int i, pid, status;

    for (i = 0; i < num_IP; i++) {
        pid = fork();
        switch (pid) {
            case -1:
                errx(1, "Error: pid < 0");
            case 0:
                printf("Soy el hijo %d\n", pid);
                if(execl("/bin/ping", "ping", /* (char)MAX_PACKETS, */"-W 5",/* (char)TIMEWAIT,*/ dir_IP[i+1], NULL) < 0) {
                    err(1,"Error: execl failed");
                }
            default:
                printf("Soy el padre %d\n", pid);
                while(wait(&status) != pid){
                    ;
                }
                if (WIFEXITED(status)) {
                    printf("Un hijo ha finalizado\n");
                }
        }
    }
}

int
main(int argc, char *argv[])
{
    int num_IP;
    num_IP = argc -1;

    if (num_IP > MAX_IPS || argc < 2) {
        errx(1,"Usage: %s IP1 IP2 ... IP10", argv[0]);
    }

    do_pings(argv, num_IP);

    exit(EXIT_SUCCESS);
}
