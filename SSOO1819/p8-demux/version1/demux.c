#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

enum{
    MIN_FILES = 3,
    MAX_FILES = 10,
    MAX_LENNAME = 256,
    BUFFER_SIZE = 1024,
    PERMS = 0644
};

void
create_pipes(int fd[][2], int numpipes)
{
    int i;
    for(i = 0; i < numpipes; i++) {
        if(pipe(fd[i]) < 0) {
            err(EXIT_FAILURE, "pipe");
        }
    }
}

void
write_on_pipes(int fd[][2], int numpipes, int blocksize)
{
    FILE *pipe_fd[numpipes];
    int i, nr, pipe_id, towrite, pos;
    char buf[BUFFER_SIZE];

    for (i = 0; i < numpipes; i++) {
        pipe_fd[i] = fdopen(fd[i][1], "w");
        if (pipe_fd[i] == NULL) {
            err(EXIT_FAILURE, "fdopen");
        }
    }
    towrite = blocksize;
    pipe_id = 0; // start writting on pipe number zero
    for(;;) {
        nr = fread(buf, sizeof(char), BUFFER_SIZE, stdin);
        printf("%s\n",buf);
        if (nr == 0) {
            if(ferror(stdin)) {
                err(EXIT_FAILURE, "fread");
            }
            break;  // EOF
        }
        pos = 0;
        for(;;) {
            if (nr <= 0) {
                break;  // lets continue reading from file in stdin
            }else if (nr >= towrite) {
                if (fwrite(&buf[pos],sizeof(char), towrite, pipe_fd[pipe_id])!= towrite) {
                    err(EXIT_FAILURE, "fwrite");
                }
                nr -= towrite;
                pos += towrite;
                towrite = blocksize;
                pipe_id = (pipe_id+1)%numpipes;
            }else{
                if (fwrite(&buf[pos],sizeof(char), nr, pipe_fd[pipe_id])!= nr) {
                    err(EXIT_FAILURE, "fwrite");
                }
                towrite -= nr;
                nr = 0;
            }
        }
    }
    for (i = 0; i < numpipes; i++) {
        fclose(pipe_fd[i]);
    }
}

void
closeall(int fd[][2], int n, int extr)
{
    int i;
    for(i = 0; i < n; i++) {
        close(fd[i][extr]);
    }
}

void
do_demux(char *files[], int numfiles, int blocksize)
{
    int fd[numfiles][2]; // We will have at most MAX_FILES pipes
    int pid, i, fd_file;

    create_pipes(fd, numfiles);
    for(i = 0; i < numfiles; i++) {
        pid = fork();
        switch (pid) {
            case -1:
                err(EXIT_FAILURE, "fork");
            case 0:
                fd_file = open(files[i], O_WRONLY|O_CREAT|O_TRUNC, PERMS); //PERMS are 0644
                if (fd_file < 0) {
                    err(EXIT_FAILURE, "open %s", files[i]);
                }
                dup2(fd_file, 1);
                close(fd_file);
                dup2(fd[i][0], 0);
                closeall(fd, numfiles, 0); //we need to close al pipes on every child (pipes are created before fork!)
                closeall(fd, numfiles, 1);
                execl("/bin/gzip", "gzip", NULL);
                err(EXIT_FAILURE, "execl");
        }
    }
    closeall(fd, numfiles, 0);
    write_on_pipes(fd, numfiles, blocksize);
    closeall(fd, numfiles, 1);

    for (i = 0; i < numfiles; i++) { //hacer un wait childs
        wait(NULL);
    }
}

int
main(int argc, char *argv[])
{
    int blocksize, numfiles, i;
    char *files[MAX_FILES];

    if (argc <= MIN_FILES || argc > MAX_FILES || !(blocksize = atoi(argv[1]))){
        errx(EXIT_FAILURE, "Usage: %s numbytes fich1.gz fich2.gz <fich3.gz> ... <fich%d.gz>", argv[0], MAX_FILES);
    }

    numfiles = argc - 2;
    for (i = 0; i < numfiles; i++) {
        files[i] = strndup(argv[i+2], MAX_LENNAME);
    }

    do_demux(files, numfiles, blocksize);

    for (i = 0; i < numfiles; i++) {
        free(files[i]);
    }

    exit(EXIT_SUCCESS);
}
