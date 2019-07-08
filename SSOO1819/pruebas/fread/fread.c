#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <unistd.h>

enum{
    BUFSIZE = 20,
};

int
main(int argc, char *argv[])
{
    char buf[BUFSIZE];
    size_t nr;

    for(;;) {
        nr = fread(buf, sizeof(char), BUFSIZE, stdin);
        printf("nr: %lu\n", nr);    // try to comment this line
        if (nr == 0) {
            if(ferror(stdin)) {
                err(EXIT_FAILURE, "fread");
            }
            break;  // EOF
        }
        if (fwrite(buf,sizeof(char), nr, stdout)!= nr) {
            err(EXIT_FAILURE, "fwrite");
        }
    }

    exit(EXIT_FAILURE);
}
