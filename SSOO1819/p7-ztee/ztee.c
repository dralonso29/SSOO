#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

enum{
        NUM_ARGS = 2,
        BUFFER_SIZE = 1024,
        PERMS = 0644,
};

void
write_on_file(char *file, int fd_pipe)
{
    int fd, nr;
    char buf[BUFFER_SIZE];

    fd = open(file, O_WRONLY|O_CREAT|O_TRUNC, PERMS); //PERMS are 0644
    if (fd < 0) {
        err(EXIT_FAILURE, "open %s", file);
    }

    for (;;) {
        nr = read(fd_pipe, buf, BUFFER_SIZE);
        if (nr < 0) {
            err(EXIT_FAILURE, "read");
        }
        if (nr == 0) {  //EOF from gunzip from pipe
            break;
        }

        if ((write(1, buf, nr)) != nr) {
            err(EXIT_FAILURE, "write on stdout");
        }

        if ((write(fd, buf, nr)) != nr) {
            err(EXIT_FAILURE, "write on %s", file);
        }
    }
    if (close(fd) < 0) {
        err(EXIT_FAILURE, "close");
    }
}

int
status_child(int pid)
{
    int wstatus;
    if(wait(&wstatus) < 0) {
        err(EXIT_FAILURE, "wait");
    }
    return WIFEXITED(wstatus) && WEXITSTATUS(wstatus);
}

int
do_tee(char *file)
{
    int pid, fd[2], status;

    if(pipe(fd) < 0) {
        err(EXIT_FAILURE, "pipe");
    }

    pid = fork();
    switch (pid) {
        case -1:
            err(EXIT_FAILURE, "fork");
        case 0:
            close(fd[0]);
            dup2(fd[1], 1);
            close(fd[1]);
            execl("/bin/gunzip", "gunzip", NULL);
            err(EXIT_FAILURE, "execl failed");
        default:
            close(fd[1]);
            write_on_file(file, fd[0]);
            close(fd[0]);
            status = status_child(pid);
    }
    return status;
}

int
main(int argc, char *argv[])
{
    int stat_exit;
    if(argc != NUM_ARGS) {
        errx(EXIT_FAILURE, "Usage: %s exit_file", argv[0]);
    }

    stat_exit = do_tee(argv[1]);
    exit(stat_exit);
}
