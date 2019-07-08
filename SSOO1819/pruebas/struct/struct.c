#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

struct list{
        int pid;
        char *fich_name;
        int unico;
};

int
main(int argc, char *argv[])
{
    int i;
    struct list my_list[argc];

    for (i = 0; i < argc-1; i++) {
        my_list[i].pid = i+1;
        my_list[i].fich_name = argv[i+1];
        my_list[i].unico = 0;
    }

    for (i = 0; i < argc-1; i++) {
        printf("pid:%d ", my_list[i].pid);
        printf("name:%s ",my_list[i].fich_name);
        printf("unico:%d\n",my_list[i].unico);
    }
    exit(EXIT_SUCCESS);
}
