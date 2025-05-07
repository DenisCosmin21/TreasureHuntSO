#include "MonitorHelpers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "operationHelpers.h"

static int startTreasureManager(void) {
    int treasureManagerPid = 0;
    treasureManagerPid = fork();
    if (treasureManagerPid < 0) {
        perror("Error forking");
        exit(3);
    }

    return treasureManagerPid;
}

static void viewTreasure(const char *huntId,const char *treasureId) {
    int treasureManagerPid = startTreasureManager();
    if (treasureManagerPid == 0) {
        if (execl("./cmake-build-debug/treasure_manager", "./treasure_manager", "--view", huntId, treasureId, NULL) < 0) {
            perror("Error starting the treausreManager process");
            exit(3);
        }
    }
    else {
        int finishStatus = 0;
        waitpid(treasureManagerPid, &finishStatus, 0);
    }
}

static void listTreasure(const char *huntId) {
    int treasureManagerPid = startTreasureManager();

    if (treasureManagerPid == 0) {
        if (execl("./cmake-build-debug/treasure_manager", "./treasure_manager", "--list", huntId, NULL) < 0) {
            perror("Error starting the treausreManager process");
            exit(3);
        }
    }
    else {
        int finishStatus = 0;
        waitpid(treasureManagerPid, &finishStatus, 0);
    }
}

static void listHunts(void) {
    int treasureManagerPid = startTreasureManager();

    if (treasureManagerPid == 0) {
        if (execl("./cmake-build-debug/treasure_manager", "./treasure_manager", "--list_hunts", NULL) < 0) {
            perror("Error starting the treausreManager process");
            exit(3);
        }
    }
    else {
        int finishStatus = 0;
        waitpid(treasureManagerPid, &finishStatus, 0);
    }
}


static void stopMonitor(void) {
    notifyProcess(getppid(), SIGUSR1);//Notify the hub process about the ending of the monitor soon
}

static void exitMonitor(int sig) {
    exit(0);
}

static void parseOperation(const char * operation, const char (*parameters)[1024], const size_t parametersCount) {
    if (strcmp(operation, "stop_monitor") == 0) {
        stopMonitor();
        return;
    }
    else if (strcmp(operation, "list_hunts") == 0) {
        listHunts();
    }
    else if (strcmp(operation, "list_treasures") == 0) {
        listTreasure(parameters[0]);
    }
    else if (strcmp(operation, "view_treasure") == 0) {
        viewTreasure(parameters[0], parameters[1]);
    }
    else {
        printf("\033[31mUnknown operation: %s\nUsage : \nstart_monitor\nlist_hunts\nlist_treasure <hunt_id>\nview_treasure <hunt_id> <treasure_id>\nstop_monitor\nexit\n\033[0m", operation);
        fflush(stdout);
    }
    notifyProcess(getppid(), SIGUSR2); //Notify the parrent that we finished parsing this, so we can continue
}

static void executeCommand(int sig){
    char (*arguments)[1024] = readOperationInfoFromFile();

    if (arguments != NULL) {
        parseOperation(arguments[1], arguments + 2, strtol(arguments[0], NULL, 10));
        //Parse the operation
    }
}

void initMonitorProcess(void) {
    struct sigaction monitorAction;

    memset(&monitorAction, 0x00, sizeof(struct sigaction));
    monitorAction.sa_handler = executeCommand;

    if (sigaction(SIGUSR1, &monitorAction, NULL) < 0) {
        perror("Error when setting a event handler for the signal SIGUSR1");
        exit(3);
    }
    struct sigaction monitorClose;

    memset(&monitorClose, 0x00, sizeof(struct sigaction));
    monitorClose.sa_handler = exitMonitor;

    if (sigaction(SIGTERM, &monitorClose, NULL) < 0) {//Handler for custom close of the process on sigterm signal
        perror("Error when setting a event handler for the signal SIGUSR1");
        exit(3);
    }
}

