#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <unistd.h>
// Programa que inicializa un array de enteros
void
editfd(int fd[])
{
    int i;
    for(i = 0; i < 10; i++) {
        fd[i] = i+1;
    }
}

int
main(int argc, char *argv[])
{
    int fd[10];
    editfd(fd);
    for (size_t i = 0; i < 10; i++) {
        printf("fd[%lu]=%d\n",i, fd[i] );
    }
}
