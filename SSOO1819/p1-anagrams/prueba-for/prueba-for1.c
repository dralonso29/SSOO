#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>

int
soniguales(char * s1, char * s2)
{
    int len = strlen(s1);
    return strncmp(s1, s2, len);
}

int
main(int argc, char *argv[]) {

    // int count = 4;
    // int count2 = 4;
    // for (int i = 0; i < count; i++) {
    //     for (int j = 0; j < count2; j++) {
    //         printf("i:%d, j:%d\n", i,j);
    //     }
    // }
    printf("%d\n", !soniguales(argv[1], argv[2]));

    exit(EXIT_SUCCESS);
}
