/*
De esta forma podemos reemplazar una variable de entorno
cuando delante de esta tenemos alguna palabra
En la version final de sh.c he puesto la version simple y ya
*/

int
replace_env(char *splited[], int len)
{
    char *dollar[2];
    char buf[MAX_NAME];
    char *env;
    char *found;
    int i, exists;

    exists = 1;
    for (i = 0; i < len; i++) {
        found = strchr(splited[i], '$');
        if (found != NULL) {
            split_by(splited[i], delim_dollar, dollar);
            if (splited[i][0] == '$') { //usar strchr para ver si esta el $ y si esta, hacer split_by del puntero que nos devuelve
                env = getenv(dollar[0]);
                if (env != NULL) {
                    splited[i] = env;
                    printf("%s\n",splited[i] );
                }else{
                    fprintf(stderr, "Error: var %s does not exist\n", splited[i]);
                    exists = 0;
                }
            }else{
                env = getenv(dollar[1]);
                if (env != NULL) {
                    snprintf(buf, MAX_NAME, "%s%s", dollar[0], env);
                    splited[i] = buf;
                    printf("%s\n",splited[i] );
                }else{
                    fprintf(stderr, "Error: var %s does not exist\n", splited[i]);
                    exists = 0;
                }
            }
        }
    }
    return exists;
}
