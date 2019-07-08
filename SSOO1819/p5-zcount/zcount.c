#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

enum{
        MAX_ARGS = 2,
        MAX_FILENAME = 256,
        MAX_SIZE_BUFFER = 512,
        MAX_PATH = MAX_FILENAME * 4,
        PERMS = 0644,
};

int
isvalidfile(char *file)
{
    return strncmp(file, ".", MAX_FILENAME) || strncmp(file, "..", MAX_FILENAME);
}

int
isregfile(unsigned char type)
{
    return type == DT_REG;
}

void
analize_buffer(char * buffer, int size, int *zeros) // fill variable zeros with number of zero bytes in buffer
{
    int i;

    for (i = 0; i < size; i++) {
        if (buffer[i] == 0) {
            (*zeros)++;
        }
    }
}

int
count_zeros(char * filename) //returns number of zero bytes of a file
{
    int zeros;
    int fd, nr;
    char buffer[MAX_SIZE_BUFFER]; //buffer where we alocate the read file

    fd = open(filename, O_RDONLY); //open file only in read mode
    if (fd < 0) {
        err(EXIT_FAILURE, "open");
    }

    zeros = 0; //At first, number of zero bytes is zero
    for (;;) {
        nr = read(fd, buffer, MAX_SIZE_BUFFER);
        if (nr < 0) {
            err(EXIT_FAILURE, "read");
        }

        if (nr == 0) { //End Of File
            break;
        }

        analize_buffer(buffer, nr, &zeros);
    }
    if (close(fd) < 0) {
        err(EXIT_FAILURE, "close");
    }
    return zeros;
}

int
are_names_equal(char * file1, char *file2)
{
    return !strncmp(file1, file2, MAX_FILENAME);
}

void
truncate_file(char * file)
{
        int fd;
        fd = open(file, O_WRONLY|O_TRUNC);
        if (fd < 0) {
            err(EXIT_FAILURE, "open");
        }
        if (close(fd)) {
            err(EXIT_FAILURE, "close");
        }
}

void
write_on_file(char *buffer, int buf_size, char *file)
{
        int fd, wr;

        fd = open(file, O_WRONLY|O_CREAT|O_APPEND, PERMS);
        if (fd < 0) {
            err(EXIT_FAILURE, "open");
        }

        wr = write(fd, buffer, buf_size);
        if (wr != buf_size) {
            err(EXIT_FAILURE, "write");
        }

        if (close(fd) < 0) {
            err(EXIT_FAILURE, "close");
        }
}

void
list_dir(char *dir)
{
    DIR * dirp;
    struct dirent *dirn;
    char path[MAX_PATH];
    char buf[MAX_SIZE_BUFFER];
    char path_dest[MAX_PATH];
    int bytes_zero;

    dirp = opendir(dir);
    if (dirp == NULL) {
        err(EXIT_FAILURE, "opendir");
    }

    snprintf(path_dest, MAX_PATH, "%s/z.txt", dir);
    while ((dirn = readdir(dirp)) != NULL) {
        if (isregfile(dirn->d_type) && isvalidfile(dirn->d_name)) {
            if (are_names_equal(dirn->d_name, "z.txt")) {
                truncate_file(path_dest);
                continue;
            }
            snprintf(path, MAX_PATH, "%s/%s", dir, dirn->d_name);
            bytes_zero = count_zeros(path);
            snprintf(buf, MAX_SIZE_BUFFER, "%d %s\n", bytes_zero, dirn->d_name);
            write_on_file(buf, strlen(buf), path_dest);
        }
    }
    closedir(dirp);
}

int
main(int argc, char *argv[])
{
    if (argc != MAX_ARGS) {
        errx(EXIT_FAILURE, "Usage: %s dir", argv[0]);
    }

    list_dir(argv[1]);

    exit(EXIT_SUCCESS);
}
