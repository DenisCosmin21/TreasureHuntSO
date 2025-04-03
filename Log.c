#include "Log.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

static void log(const int fd, const char * message) {
    if (write(fd, message, strlen(message)) < 0) {
        perror("Unable to write to log");
        exit(1);
    }
}

static char * getLogFilePath(const char *path, char * fullPath) {
    strcat(fullPath, path);
    strcat(fullPath, "/");
    strcat(fullPath, baseName);
    //Get the full path of the file descriptor in wich you should add the log

    return fullPath;
}

static int openLogFile(const char *path) {
    int fd = -1;

    char fullPath[100] = {0};

    if (path == NULL) {
         fd = open(baseName, O_CREAT | O_APPEND | O_WRONLY, S_IRUSR | S_IWUSR | S_IROTH);
    }
    else {
        //Check if main log file exists, else create it
        if (stat(baseName, NULL) < 0)
            close(openLogFile(NULL));


        getLogFilePath(path, fullPath);


        struct stat st;

        if (lstat(fullPath, &st) < 0) { //Symbolic link doesn't exist so need to create it

            char baseFileName[100] = {0};
            strcpy(baseFileName, "../");
            strcat(baseFileName, baseName); //Path of the file relative to the folder where i want to make the symlink

            if (symlink(baseFileName, fullPath) < 0) {
                perror("Unable to open the symbolic link file");
                exit(1);
            }
        }

        fd = open(fullPath, O_WRONLY | O_APPEND);

    }

    if (fd < 0) {
        perror("Log file imposible to open");
        exit(1);
    }

    return fd;
}

void logInfo(const char *message, const char *path){
    const int fd = openLogFile(path);

    write(fd, "[INFO LOG] : ", 12);

    log(fd, message);

    close(fd);
}

void logError(const char *message, const char *path) {
    const int fd = openLogFile(path);

    write(fd, "[ERROR LOG] : ", 12);

    log(fd, message);

    close(fd);
}

void logWarning(const char *message, const char *path) {
    const int fd = openLogFile(path);

    write(fd, "[WARNING LOG] : ", 12);

    log(fd, message);

    close(fd);
}
