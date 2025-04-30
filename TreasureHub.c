#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "treasureMonitor.h"


int main(void) {
    initTreasureHubProcess();
    promptForCommand();
    while (1);
}

/*char **arguments = splitArguments(operation);
        printf("%s\n", arguments[0]);
        if (arguments != NULL) {
            //Then parse operation else wait for further instructions
            parseOperation(arguments[1], arguments + 2, strtol(arguments[0], NULL, 10));
        }*/