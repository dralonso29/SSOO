#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

void
suma(int * numero)
{
    *numero = 7;
}

int
main(int argc, char *argv[])
{
    int numero = 5;

    suma(&numero);
    printf("%d\n", numero);

    exit(0);
}
