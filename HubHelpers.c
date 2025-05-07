#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "HubHelpers.h"
#include "operationHelpers.h"
#include <string.h>
#include <sys/wait.h>

int monitorStarted = 0;
int monitorPid = 0;
int monitorKillerPid = 0;

void executeOperation(const char *operation) {
    if (!monitorStarted) {//If monitor is not started yet we can't execute any operation
        if (monitorPid != 0) {
            printf("\033[31mMonitor process not closed yet. Wait for it to close\033[0m\n");
        }
        else {
            printf("\033[31mMonitor not started\n\033[0m");//Add colored writing to not write to stderr becouse of the time it takes to wrtie to it, so it won't sync that great with main printf to stdout, even with flush
        }

        notifyProcess(getpid(), SIGUSR2);
        return;
    }
    writeOperationInfoInFile(operation);//Write the operation in the file

    notifyProcess(monitorPid, SIGUSR1);//notify the process about the new operation
}

static void promptForCommand(int sig) {
    if (monitorPid != 0 || (monitorPid == 0 && monitorStarted == 0)) {
        char operation[1024];

        printf("Insert operation : ");
        fflush(stdout);

        if (fgets(operation, 1024, stdin) == NULL) {
            notifyProcess(getpid(), SIGUSR2); //Notify it to read again from stdin, becouse we paused the stdin read
        }
        else {
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
    }
}

static void closeMonitorAction(int sig) {
    if (monitorKillerPid != 0) {
        int status = 0;
        if (waitpid(monitorPid, &status, 0) < 0) {
            perror("Erorr when waiting for moitor to close");
            exit(3);
        }//Did wait to remove the process from zombie state

        if (waitpid(monitorKillerPid, &status, 0) < 0) {
            perror("Erorr when waiting for moitor killer to close");
            exit(3);
        }//Did wait to remove the process from zombie state
        int c;
        printf("\nMonitor closed\n");

        monitorPid = 0; //Reset the monitor pid so that we know that we don;t have the monitor process on
        monitorStarted = 0;
        monitorKillerPid = 0;
    }
}

static void killMonitor(int sig) {
    usleep(1000000);
    notifyProcess(monitorPid, SIGTERM);
    exit(0);
}

static void initMonitorKiller(void) {
    struct sigaction monitorAction;
    memset(&monitorAction, 0x00, sizeof(struct sigaction));

    monitorAction.sa_handler = killMonitor;
    if (sigaction(SIGUSR1, &monitorAction, NULL) < 0) {
        perror("Error when setting a event handler for the signal SIGUSR1");
        exit(3);
    }
}

static void startMonitorCloseAction(int sig) {
    monitorStarted = 0;
    monitorKillerPid = fork();
    if (monitorKillerPid < 0) {
        perror("Error forking");
        exit(3);
    }
    if (monitorKillerPid == 0) {
        initMonitorKiller();
    }
    else {
        notifyProcess(monitorKillerPid, SIGUSR1);
        notifyProcess(getpid(), SIGUSR2);
    }
}

void initTreasureHubProcess(void) {
    struct sigaction monitorClosingAction;
    memset(&monitorClosingAction, 0x00, sizeof(struct sigaction));

    monitorClosingAction.sa_handler = startMonitorCloseAction;

    if (sigaction(SIGUSR1, &monitorClosingAction, NULL) < 0) {
        perror("Error when setting event handler for hub");
        exit(3);
    }

    struct sigaction hubAction;

    memset(&hubAction, 0x00, sizeof(struct sigaction));

    hubAction.sa_handler = promptForCommand;

    if (sigaction(SIGUSR2, &hubAction, NULL) < 0) {
        perror("Error when setting event handler for hub");
        exit(3);
    }

    struct sigaction monitorClosedAction;

    memset(&monitorClosedAction, 0x00, sizeof(struct sigaction));
    monitorClosedAction.sa_handler = closeMonitorAction;

    if (sigaction(SIGCHLD, &monitorClosedAction, NULL) < 0) {
        perror("Error when setting event handler for monitor stop on hub");
        exit(3);
    }

    notifyProcess(getpid(), SIGUSR2);
}

void startMonitor(void) {

    if (!monitorStarted) {
        printf("Starting monitor\n");
        monitorPid = fork();
        if (monitorPid < 0) {
            perror("Error forking");
            exit(3);
        }
        if (monitorPid == 0)
            if(execl("./cmake-build-debug/monitor", NULL) < 0){
              perror("Error starting monitor");
              exit(3);
            }

        monitorStarted = 1;//Flag to know that we have the monitor running
        printf("Monitor started\n");
    }
    else {
        printf("\033[31mMonitor already started]\n\033[0m");
    }

    notifyProcess(getpid(), SIGUSR2);
}

void exitHub(void) {
    if (!monitorStarted && monitorPid == 0) {
        printf("Exiting hub\n");
        exit(0);
    }

    printf("\033[31mMonitor still running\033[0m\n");

    notifyProcess(getpid(), SIGUSR2);
}
