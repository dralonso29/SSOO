#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

char *
env_var(char * var) // devuelve null o algo distinto a null si existe var como variable de entorno
{
    return getenv(var);
}

void
tokenize_var(char * content, char * delim) //separa el string "content" por el delimitardor "delim"
{
    char * token;

    token = strtok(content, delim);
    while(token != NULL) {
        printf("%s\n", token);
        token = strtok(NULL, delim);
    }
}

int
main(int argc, char *argv[]) {
    int i;
    char delim[] = ":";
    char * content;

    for (i = 1; i < argc; i++) {
        content = env_var(argv[i]); //si el contenido de la variable de entorno es null, salimos por stderr
        if(content != NULL) {
            tokenize_var(content, delim); //tokenizamos el contenido de la variable de entorno
        }else{  //tambien podemos usar errx();
            fprintf(stderr, "ERROR: var %s does not exist\n", argv[i]);
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}
