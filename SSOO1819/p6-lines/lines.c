#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

enum{
        NUM_ARGS = 3,
        MAX_FILENAME = 256,
        MAX_NUM_FILES = 50,
        MAX_PATH_SIZE = 512,
        PERMS = 0644,
};

void
rm_file(char *route, char *filename)    //delete file if exists
{
    char path[MAX_PATH_SIZE];
    int ul;
    struct stat statbuf;

    snprintf(path, MAX_PATH_SIZE, "%s/%s", route, filename);
    if (stat(path, &statbuf)) {
        err(EXIT_FAILURE, "stat");
    }

    if ((statbuf.st_mode & S_IFMT) == S_IFREG) {
        ul = unlink(path);
        if (ul < 0) {
            err(EXIT_FAILURE, "unlink");
        }
    }
}

int
isregfile(unsigned char type)
{
    return type == DT_REG;
}

int
is_extension_eq(char *name, char *ext)  //return true if extension ext is found in name
{
    int lenname, lenext;
    char *locurr;

    lenname = strnlen(name, MAX_FILENAME);
    lenext = strnlen(ext, MAX_FILENAME);

    locurr = strrchr(name, '.');
    return (lenname > lenext) && (locurr != NULL) && !strncmp(locurr, ext, lenname);
}

int
find_files(char *dir, char *files[])
{
        DIR * dirp;
        struct dirent *dirn;
        int i;

        dirp = opendir(dir);
        if (dirp == NULL) {
            err(EXIT_FAILURE, "opendir");
        }

        i = 0;
        while ((dirn = readdir(dirp)) != NULL) {
            if (isregfile(dirn->d_type) && is_extension_eq(dirn->d_name, ".txt")) {
                files[i] = dirn->d_name;
                i++;
            }
        }
        closedir(dirp);
        return i;
}

void
wait_child(int child_pid, char *route)
{
    int wstatus;
    if(waitpid(child_pid, &wstatus, 0) < 0) {
        err(EXIT_FAILURE, "waitpid");
    }

    if (WIFEXITED(wstatus) && (WEXITSTATUS(wstatus) == 2)) {
        rm_file(route, "/../lines.out");
        errx(EXIT_FAILURE, "fgrep failed");
    }
}

void
do_fgrep(char *files[], int num_files, char *mword, char *route)
{
    int i, pid, fd, fd_dn;
    char parent[MAX_PATH_SIZE];
    char path[MAX_PATH_SIZE];

    snprintf(parent, MAX_PATH_SIZE, "%s/../lines.out", route);
    fd = open(parent, O_WRONLY|O_CREAT|O_TRUNC, PERMS);
    if (fd < 0) {
        err(EXIT_FAILURE, "open");
    }

    for(i = 0; i<num_files; i++){
        pid = fork();
        switch (pid) {
            case -1:
                err(EXIT_FAILURE, "fork");
            case 0:
                dup2(fd, 1);
                close(fd);

                fd_dn = open("/dev/null", O_WRONLY);
                if (fd_dn < 0) {
                    err(EXIT_FAILURE, "open");
                }
                dup2(fd_dn, 2);
                close(fd_dn);

                snprintf(path, MAX_PATH_SIZE, "%s/%s", route, files[i]);
                execl("/bin/fgrep", "fgrep", mword, path, NULL);
                err(EXIT_FAILURE, "execl");
            default:
                wait_child(pid, route);
        }
    }
    close(fd);
}

int
main(int argc, char * argv[])
{
    char *files[MAX_NUM_FILES];
    int num_files;

    if (argc != NUM_ARGS) {
        errx(EXIT_FAILURE, "Usage: %s word dir", argv[0]);
    }

    num_files = find_files(argv[2], files);
    do_fgrep(files, num_files, argv[1], argv[2]);

    exit(EXIT_SUCCESS);
}
