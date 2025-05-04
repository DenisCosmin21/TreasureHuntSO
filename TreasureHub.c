#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "treasureMonitor.h"


int main(void) {
    initTreasureHubProcess();

    while (1) {//Main process loop
        while (!monitorStarted && monitorPid == 0) {
            //While the monitor is not started show the prompt
            promptForCommand(0);
        }
    }

    return 0;
}
