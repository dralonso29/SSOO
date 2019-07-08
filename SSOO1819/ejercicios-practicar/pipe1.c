#include <stdio.h>
#include <stdlib.h>
#include <err.h>

enum{
        MIN_ARGC = 2,
};

int
main(int argc, char *argv[])
{
    char *cmd1[MAX_CMD];
    char *cmd2[MAX_CMD];

    if (argc < MIN_ARGC) {
        errx(EXIT_FAILURE, "Usage: %s cmd1 <flags> | cmd2 <flags>", argv[0]);
    }
    argv++;
    argc--;
    for (size_t i = 0; i < argc; i++) {
        if(strlen(argv[i]) != 1 && argv[i][0] != '|'){
            cmd1[i] = argv[i];
        }else{
            break;
        }
    }
    exit(EXIT_SUCCESS);
}
