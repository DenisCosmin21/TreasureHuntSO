#define _POSIX_C_SOURCE 200809L
#include "operationHelpers.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include "DirectoryLib.h"
#include "FileLib.h"
#include <string.h>

static char (*splitArguments(char *operation))[1024] {//Split the arguments to mimic the argv format
    static char arguments[4][1024]; //The operations that we should be able to parse have at most 3 parameters. First one will be the argCount so that we know without going over it again
    char *param = strtok(operation, " ");

    if (param == NULL) {
        param = strtok(operation, "\n");
    }

    size_t cnt = 1; //postiion of the arguments parsed
    while (param != NULL) {
        strcpy(arguments[cnt++], param);
        if (cnt > 4) {
            fprintf(stderr, "Too many arguments\n");
            return NULL;
        }
        param = strtok(NULL, " ");
    }

    sprintf(arguments[0], "%ld", cnt);
    //printf("%s\n%s\n%s\n%s\n", arguments[0], arguments[1], arguments[2], arguments[3]);
    return arguments;
}

void notifyProcess(const int processPid, const int signal) {
    if (kill(processPid, signal) < 0) {//SIGUSR1 means that an operaion is recieved
        perror("Impossible to send a signal to monitor");
        exit(3);
    }
}

void writeOperationInfoInFile(const char *operation) {

    if (!existsDirectory("insides")) {
        createDirectory("insides");
    }//First create the directory insides if it doesn't exist

    int fd = openFile("insides/operations.in", "wt"); //The operation will be written to the file operations from folder insides
    //insides folder will contain program specific files needed to proprrly function
    //we only need one operation at a time in the file so we open it on write mode
    writeFile(fd, operation, strlen(operation) - 1); //-1 to remove the /n

    closeFile(fd);
}

char (*readOperationInfoFromFile(void))[1024] {
    //If the signal SIGUSR1 is recieved we are sure that the monitor process is running
    int fd = openFile("insides/operations.in", "r");
    char operationBuffer[1024] = {0};

    readFile(fd, operationBuffer, 1024);
    closeFile(fd);

    //After reading the operation we need to parse it so that we make it the same way command line arguments are

    return splitArguments(operationBuffer);
}
