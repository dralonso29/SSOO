// EXAMPLE
//        One example of use is the following code, which simulates typing
//
//            ls -l *.c ../*.c
//
//        in the shell:
//
           // glob_t globbuf;
           //
           // globbuf.gl_offs = 2;
           // glob("*.c", GLOB_DOOFFS, NULL, &globbuf);
           // glob("../*.c", GLOB_DOOFFS | GLOB_APPEND, NULL, &globbuf);
           // globbuf.gl_pathv[0] = "ls";
           // globbuf.gl_pathv[1] = "-l";
//            execvp("ls", &globbuf.gl_pathv[0]);

#include <stdio.h>
#include <stdlib.h>
#include <glob.h>

int
main(int argc, char *argv[])
{
    /*LA FUNCION LA TENGO IMPLEMENTADA EN LA PRACTICA FINAL DE SSOO. LA FUNCION SE LLAMA replace_glob*/
    glob_t globbuf;
    char *list[3]= {"ls", "*.c"};
    // int j;

    globbuf.gl_offs = 0;
    globbuf.gl_pathv = NULL;
    for (size_t i = 0; i < 3; i++) {
        if (glob(list[i], GLOB_DOOFFS|GLOB_NOCHECK, NULL, &globbuf) == 0) {
            printf("gl_pathc: %lu\n", globbuf.gl_pathc);
            for (size_t j = 0; j < globbuf.gl_pathc; j++) {
                printf("%s\n",globbuf.gl_pathv[j]);
                // list[i] = globbuf.gl_pathv[i];
            }
        }
    }

    // while (list[j] != NULL) {
    //     printf("%s\n",list[j]);
    //     j++;
    // }
    // glob("../*.c", GLOB_DOOFFS | GLOB_APPEND, NULL, &globbuf);
    // globbuf.gl_pathv[0] = "ls";
    // globbuf.gl_pathv[1] = "-l";


    exit(EXIT_SUCCESS);
}
