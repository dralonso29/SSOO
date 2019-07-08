#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>

enum {
    MAX_PARAM = 101,    //Numero maximo de parametros
    MIN_PARAM = 3            //Numero minimo de parametros
};

void
verifyargs(int argc, char * argv[])
{
    if (argc > MAX_PARAM) {
        errx(2, "Error: Must input less arguments. Limit: %d", MAX_PARAM - 1);
    }else if (argc < MIN_PARAM) {
        errx(2, "Usage: %s [arg1] [arg2] ... [arg100]", argv[0]);
    }
}

int
mismolen(int len1, int len2)
{
    return len1 == len2;
}

void
reset_array(int * array, int len)
{
    memset(array, 0, sizeof(int)*len);
}

int
mismoschars(char * s1, char * s2, int len1, int len2) //devuelve 1 si son anagramas, 0 si no lo son
{
    int soniguales = 1; //suponemos que van a ser anagramas
    char * paux = strdup(s2);

    for (int i = 0; i < len1; i++) {
        for (int j = 0; j < len2; j++) {
            if (s1[i] == paux[j]) {
                paux[j] = '@';
                break;
            }else if (s1[i] != paux[j] && j == len2 -1) {
                soniguales = 0;
                free(paux);
                return soniguales;
            }
        }
    }
    free(paux);
    return soniguales;
}

int
sonanagramas(char * s1, char * s2)
{
    int len_s1 = strlen(s1);
    int len_s2 = strlen(s2);
    int sonanagr = 0;   // de primeras suponemos que no son iguales

    if (mismolen(len_s1, len_s2)) {
        sonanagr = mismoschars(s1, s2, len_s1, len_s2);
        return sonanagr;
    }
    return sonanagr;
}

void
print_diferent(char * s1, char * argv[], int argc, int * arr_anag, int i)
{
    int j, k;
    char * s2;
    int len = strlen(s1);

    for (j = 0; j < argc; j++) {
        if (i<j && arr_anag[j]) {
            s2 = argv[j];
        }
    }

    printf("[");
    for (k = 0; k < len; k++) {
        if (s1[k] != s2[k]) {
            printf("%c", s1[k]);
        }
    }
    printf("]\n");
}

void
print_anagrams(char * argv[], int argc, int * arr_anag, int * printed, int i)
{
    int j, k;

    for (j = 1; j < argc; j++) {    //aqui rellenamos el array de anagramas y de los ya impresos
        if (i<j && sonanagramas(argv[i], argv[j])) {
            arr_anag[i] = 1;
            arr_anag[j] = 1;
            printed[i] = 1;
            printed[j] = 1;
        }
    }

    for(k = 1;k < argc;k++) {
        if (arr_anag[k]) {
            printf("%s ", argv[k]);
        }
    }
}

int
main(int argc, char *argv[])
{
    int i;
    int * arr_anag; //array que nos sirve para imprimir los anagramas
    int * printed;  //array que nos sirve para no volver a imprimir anagramas ya mostrados por pantalla

    verifyargs(argc, argv); // primero miramos si nos han pasado mas de MAX_PARAM parametros

    arr_anag = malloc(sizeof(int)*argc);
    printed = malloc(sizeof(int)*argc);

    reset_array(arr_anag,argc); //ponemos el array de anagramas a cero
    reset_array(printed,argc);

    for (i = 1; i < argc; i++) { //realmente el ultimo parametro no deberia comprobar nada (argc - 1)
        reset_array(arr_anag,argc);
        if(!printed[i]) {
            print_anagrams(argv, argc, arr_anag, printed, i);
            if (arr_anag[i]) { //si el argumento no se ha incluido en ninguna lista de anagramas, pasamos al siguiente
                print_diferent(argv[i], argv, argc, arr_anag, i);
            }

        }
    }
    free(arr_anag);
    free(printed);

    exit(EXIT_SUCCESS);
}
