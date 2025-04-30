#include "treasureMonitor.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "operationHelpers.h"

int monitorStarted = 0;
int monitorPid = 0;

void executeOperation(const char *operation) {
    if (!monitorStarted) {//If monitor is not started yet we can't execute any operation
        fprintf(stderr, "Monitor not started\n");
        return;
    }

    writeOperationInfoInFile(operation);//Write the operation in the file

    notifyProcess(monitorPid, SIGUSR1);//notify the process about the new operation
}

void initMonitorProcess(void) {
    struct sigaction monitorAction;
    memset(&monitorAction, 0x00, sizeof(struct sigaction));
    monitorAction.sa_handler = readOperationInfoFromFile;

    if (sigaction(SIGUSR1, &monitorAction, NULL) < 0) {
        perror("Error when setting a event handler for the signal SIGUSR1");
        exit(3);
    }
}

void promptForCommand(void) {
    char operation[1024];

    printf("Insert operation : ");
    fgets(operation, 1024, stdin);
    if (strcmp(operation, "start_monitor\n") == 0) { //One \n at the end becouse of how it reads from console when inserting enter
        startMonitor();
    }
    else if (strcmp(operation, "exit\n") == 0) {
        exitHub();
    }
    else {
        executeOperation(operation);
    }
}

void initTreasureHubProcess(void) {
    struct sigaction hubAction;

    memset(&hubAction, 0x00, sizeof(struct sigaction));

    hubAction.sa_handler = promptForCommand;

    if (sigaction(SIGUSR2, &hubAction, NULL) < 0) {
        perror("Error when setting event handler for hub");
        exit(3);
    }
}

void startMonitor(void) {
    if (!monitorStarted) {
        monitorStarted = 1;//Flag to know that we have the monitor running
        monitorPid = fork();
        if (monitorPid < 0) {
            perror("Error forking");
            exit(3);
        }
        if (monitorPid == 0) {
            initMonitorProcess();
            while (1); //Keep the kid inside this loop after starting the monitor process and added handlers
        }
        else
            printf("Monitor started\n");

    }
    else {
        perror("Monitor already started");
        return;
    }
    notifyProcess(getpid(), SIGUSR2);
}

void viewTreasure(char *huntId, char *treasureId) {
    if (!monitorStarted) {
        fprintf(stderr, "Monitor not runninng\n");
        return;
    }
    else {
        int treasureManagerPid = fork();
        if (treasureManagerPid < 0) {
            perror("Error forking");
            exit(3);
        }
        else {
            if (execl("./treasureManager", "./treasureManager", "--view_treasure", huntId, treasureId, NULL) < 0) {
                perror("Error starting the treausreManager process");
                exit(3);
            }
        }
    }
}

void stopMonitor(void) {
    int finishState = 0;

    if (monitorStarted) {
        finishState = kill(monitorPid, SIGSTOP);//Send the signal to stop monitor
        if (finishState < 0) {
            perror("Error when stopping monitor");
            exit(3);
        }
    }
    else {
        fprintf(stderr, "Monitor already stopped\n");
        return;
    }
    kill(getpid(), SIGUSR2);
}

void exitHub(void) {
    if (!monitorStarted) {
        exit(0);
    }
    else {
        perror("Monitor still running");
        return;
    }
}



