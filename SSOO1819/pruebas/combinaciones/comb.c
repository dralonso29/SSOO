#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void
comparar(int num, char *fich[])
{
    int i,j;

    for (i = 0; i < num; i++) {
        for (j = 0; j < num; j++) {
            if(i<j) {
                printf("%d%s %d%s\n",i,fich[i+1], j,fich[j+1]);
                execl("/usr/bin/cmp", "cmp", "-s", fich[i+1], fich[j+1], NULL);
                errx(EXIT_FAILURE, "exec");
            }
        }
    }
}

int
main(int argc, char *argv[])
{
    int num_fich = argc -1;

    comparar(num_fich, argv--);
    exit(EXIT_SUCCESS);
}
