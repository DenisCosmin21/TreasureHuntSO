#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>


int openFile(const char *path, const char *mode) { //Mode as characters, r => read, w => write, a => append, etc..
    int openFlags = O_CREAT;

    if (strcmp(mode, "r") == 0) openFlags |= O_RDONLY;
    else if (strcmp(mode, "rw") == 0 || strcmp(mode, "wr") == 0) openFlags |= O_RDWR;
    else if (strcmp(mode, "ra") == 0 || strcmp(mode, "ar") == 0) openFlags |= O_RDWR | O_APPEND;
    else if (strcmp(mode, "w") == 0) openFlags |= O_WRONLY;
    else if (strcmp(mode, "a") == 0) openFlags |= O_APPEND | O_WRONLY;
    else if (strcmp(mode, "t") == 0) openFlags |= O_TRUNC;
    else if (strcmp(mode, "wt") == 0 || strcmp(mode, "tw") == 0) openFlags |= O_WRONLY | O_TRUNC;

    int fd = open(path, openFlags, S_IWUSR | S_IRUSR | S_IROTH);

    if (fd == -1) {
        perror("Error in oppening file");
        exit(1);
    }

    return fd;
}

void closeFile(const int fd) {
    if (close(fd) == -1) {
        perror("Error in closing file");
        exit(1);
    }
}

void writeFile(const int fd, const void *data, const size_t len) {
    if (write(fd, data, len) < 0) {
        perror("Error writing to file");
        exit(1);
    }
}

ssize_t readFile(const int fd, void *buffer, const size_t len) {
    ssize_t count = 0;

    if ((count = read(fd, buffer, len)) < 0) {
        perror("Error reading from file");
        exit(1);
    }

    return count;
}

void makeSymlink(const char *from, const char *to) {
    if (symlink(from, to) == -1) {
        perror("Error making symlink");
        exit(1);
    }
}

int existsFile(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

struct stat getFileStat(const char *path) {
    struct stat st;
    if (stat(path, &st) == -1) {
        perror("Error getting stat");
        exit(1);
    }

    return st;
}

struct stat getFdStat(const int fd) {
    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("Error getting stat");
        exit(1);
    }

    return st;
}

void seekCursor(const int fd, const size_t offset, const int whence) {
    if (lseek(fd, offset, whence) == -1) {
        perror("Error seeking cursor");
        exit(1);
    }
}

size_t getFileSize(const int fd) {
    const struct stat st = getFdStat(fd);

    return st.st_size;
}

void truncateFile(const int fd, const __off_t length) {
    if (ftruncate(fd, length) == -1) {
        perror("Impossible to truncate file");
        exit(-1);
    }
}

struct timespec getFileTime(const int fd) {
    const struct stat st = getFdStat(fd);

    return st.st_mtim;
}

char *getFileHumanReadableTime(const int fd) {
    struct timespec t = {0, 0};

    t = getFileTime(fd);

    return ctime(&(t.tv_sec));
}




