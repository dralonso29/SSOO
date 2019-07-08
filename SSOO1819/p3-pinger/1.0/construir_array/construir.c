#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


int
main(int argc, char *argv[])
{
    char program_name[] = "ping";
    char flag1[] = "-c";
    char pack[] = "1";
    char flag2[] = "-W";
    char timer[] = "5";
    char *array[5];

    array[0] = program_name;
    array[1] = flag1;
    array[2] = pack;
    array[3] = flag2;
    array[4] = timer;
    array[5] = NULL;

    for (int i = 0; i < 6; i++) {
        printf("%d: ", i);
        if (array[i] != NULL) {
            printf("%s\n", array[i]);
        }
    }


    exit(EXIT_SUCCESS);
}
