#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

enum{
        MAX_SIZE = 20,
};

int
son_iguales(char *name1, char *name2)
{
    return !strncmp(name1, name2, MAX_SIZE);
}

void
comparar(char *fich1, char *arr[], int total)
{
    int i;

    for (i = 0; i < total; i++) {
        printf("%s %s ",fich1, arr[i+1]);
        if (!son_iguales(fich1,arr[i+1])) { //devuelve 0 si son iguales
            printf("No son iguales\n");
        }
    }
}


int
main(int argc, char *argv[])
{
    char * fich1 = "a.txt";
    int total = argc -1;
    int a = 1;

    if (argc > 2) {
        comparar(fich1, argv, total);
        printf("\n");
    }

    if (a) {
        printf("XXX\n");
    }

    exit(EXIT_SUCCESS);
}
