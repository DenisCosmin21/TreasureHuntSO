#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "HubHelpers.h"
#include "operationHelpers.h"
#include <string.h>
#include <sys/wait.h>
#include "FileLib.h"

int monitorStarted = 0;
int monitorPid = 0;
int pfd[2] = {0};

void executeOperation(const char *operation) {
    if (!monitorStarted) {//If monitor is not started yet we can't execute any operation
        if (monitorPid != 0) {
            printf("\033[31mMonitor process not closed yet. Wait for it to close\033[0m\n");
        }
        else {
            printf("\033[31mMonitor not started\n\033[0m");//Add colored writing to not write to stderr becouse of the time it takes to wrtie to it, so it won't sync that great with main printf to stdout, even with flush
        }

        return;
    }
    writeOperationInfoInFile(operation);//Write the operation in the file

    notifyProcess(monitorPid, SIGUSR1);//notify the process about the new operation
    if (strcmp(operation, "stop_monitor\n") == 0)
        return;
    char buffer[513]; //512 is the max size of the pipe

    ssize_t count = 0;

    short endRead = 0;

    while ((count = readFile(pfd[0], buffer, sizeof(buffer) - 1)) > 0) {
        buffer[count] = '\0';

        if (buffer[count - 1] == 0x04) {
            endRead = 1;
            buffer[count - 1] = '\0';
        }

        printf("%s", buffer);
        fflush(stdout);
        if (endRead)
            break;
    }
    printf("\n");
}

void promptForCommand() {
    if (monitorPid != 0 || (monitorPid == 0 && monitorStarted == 0)) {
        char operation[1024];

        printf("Insert operation : ");
        fflush(stdout);

        if(fgets(operation, 1024, stdin) == NULL) {
            return; //So that we prompt agains
        }

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

static void closeMonitorAction(int sig) {
    int status = 0;
    if (waitpid(monitorPid, &status, 0) < 0) {
        perror("Erorr when waiting for moitor to close");
        exit(3);
    }//Did wait to remove the process from zombie state

    printf("\nMonitor closed\n");

    monitorPid = 0; //Reset the monitor pid so that we know that we don;t have the monitor process on
    monitorStarted = 0;
}

static void startMonitorCloseAction(int sig) {
    printf("Closing monitor\n");
    monitorStarted = 0;
    notifyProcess(monitorPid, SIGTERM);
}

void initTreasureHubProcess(void) {
    struct sigaction monitorClosingAction;
    memset(&monitorClosingAction, 0x00, sizeof(struct sigaction));

    monitorClosingAction.sa_handler = startMonitorCloseAction;

    if (sigaction(SIGUSR1, &monitorClosingAction, NULL) < 0) {
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
}

void startMonitor(void) {
    if (!monitorStarted) {
        if (pipe(pfd) < 0) {//Get pipe fd
            perror("Error pipeing");
            exit(3);
        }

        printf("Starting monitor\n");
        monitorPid = fork();
        if (monitorPid < 0) {
            perror("Error forking");
            exit(3);
        }
        if (monitorPid == 0) {
            closeFile(pfd[0]); //Close the read fd

            dup2(pfd[1], STDOUT_FILENO); //Redirect stdout to pipe write fd

            closeFile(pfd[1]);

            if(execl("./monitor", NULL) < 0){
                perror("Error starting monitor");
                exit(3);
            }
        }

        closeFile(pfd[1]);

        monitorStarted = 1;//Flag to know that we have the monitor running
        printf("Monitor started\n");
    }
    else {
        printf("\033[31mMonitor already started]\n\033[0m");
    }
}

void exitHub(void) {
    if (!monitorStarted && monitorPid == 0) {
        printf("Exiting hub\n");
        exit(0);
    }

    printf("\033[31mMonitor still running\033[0m\n");
}
