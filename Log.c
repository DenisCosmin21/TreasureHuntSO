#include "Log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FileLib.h"
#include <unistd.h>

static void log(const int fd, const char * message) {
    writeFile(fd, message, strlen(message));

    writeFile(fd, "\n", 1);
}

static char * getLogFilePath(const char *path, char * fullPath) {
    strcpy(fullPath, path);
    //Get the full path of the file descriptor in wich you should add the log
    strcat(fullPath, ".log");
    return fullPath;
}

static int openLogFile(const char *path) {
    int fd = -1;

    char fullPath[100] = {0};

    if (path == NULL) {
        char logFileName[64] = {0};
        strcpy(logFileName, baseName);
        strcat(logFileName, ".log");

         fd = openFile(logFileName, "a");
    }
    else {
        //Check if main log file exists, else create it
        if (!existsFile(baseName))
            closeFile(openLogFile(NULL)); //If OpenLogFile is called with null parameter it creates the main log file


        getLogFilePath(path, fullPath);

        if (!existsFile(fullPath)) { //Symbolic link doesn't exist so need to create it

            char baseFileName[100] = {0};
            strcpy(baseFileName, "../");
            strcat(baseFileName, baseName); //Path of the file relative to the folder where i want to make the symlink
            strcat(baseFileName, ".log");

            makeSymlink(baseFileName, fullPath);
        }

        fd = openFile(fullPath, "a");
    }

    return fd;
}

void logInfo(const char *message, const char *path){
    const int fd = openLogFile(path);

    write(fd, "[INFO LOG] : ", 13);

    log(fd, message);

    closeFile(fd);
}

void logError(const char *message, const char *path) {
    const int fd = openLogFile(path);

    write(fd, "[ERROR LOG] : ", 14);

    log(fd, message);

    closeFile(fd);
}

void logWarning(const char *message, const char *path) {
    const int fd = openLogFile(path);

    write(fd, "[WARNING LOG] : ", 16);

    log(fd, message);

    closeFile(fd);
}

void logSuccess(const char *message, const char *path) {
    const int fd = openLogFile(path);

    write(fd, "[SUCCESS LOG] : ", 16); //Add the color green for specific log type

    log(fd, message);

    closeFile(fd);
}
