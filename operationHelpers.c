#include "operationHelpers.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "FileLib.h"
#include "DirectoryLib.h"

static char (*splitArguments(char *operation))[1024] {//Split the arguments to mimic the argv format
    static char arguments[4][1024]; //The operations that we should be able to parse have at most 3 parameters. First one will be the argCount so that we know without going over it again
    char *param = strtok(operation, " ");

    if (param == NULL) {
        param = strtok(operation, "\n");
    }

    size_t cnt = 1; //postiion of the arguments parsed
    while (param != NULL) {
        strcpy(arguments[cnt++], param);
        if (cnt > 3) {
            fprintf(stderr, "Too many arguments\n");
            return NULL;
        }
        param = strtok(NULL, " ");
    }

    sprintf(arguments[0], "%ld", cnt);

    return arguments;
}

static void parseOperation(const char * operation, const char (*parameters)[1024], const size_t parametersCount) {
    if (strcmp(operation, "stop_monitor") == 0) {
        printf("Stopping monitor");
    }
    else if (strcmp(operation, "list_hunts") == 0) {
        printf("Listing hunts");
    }
    else if (strcmp(operation, "list_treasures") == 0) {
        printf("Listing treasures");
    }
    else if (strcmp(operation, "view_treasure") == 0) {
        printf("Viewing treasure");
    }
    else {
        fprintf(stderr, "Unknown operation: %s\nUsage : \nstart_monitor\nlist_hunts\nlist_treasure <hunt_id>\nview_treasure <hunt_id> <treasure_id>\nstop_monitor\nexit\n", operation);
    }
    fflush(stderr);
    notifyProcess(getppid(), SIGUSR2); //Notify the parrent that we finished parsing this, so we can continue
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

void notifyProcess(const int processPid, const int signal) {
    if (kill(processPid, signal) < 0) {//SIGUSR1 means that an operaion is recieved
        perror("Impossible to send a signal to monitor");
        exit(3);
    }
}

void readOperationInfoFromFile(void) {
    //If the signal SIGUSR1 is recieved we are sure that the monitor process is running
    int fd = openFile("insides/operations.in", "r");
    char operationBuffer[1024] = {0};

    readFile(fd, operationBuffer, 1024);
    closeFile(fd);

    //After reading the operation we need to parse it so that we make it the same way command line arguments are
    char (*operationParameters)[1024] = splitArguments(operationBuffer);

    if (operationParameters != NULL) {
        parseOperation(operationParameters[1], operationParameters + 2, strtol(operationParameters[0], NULL, 10));
        //Parse the operation
    }
}
