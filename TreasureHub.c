#include "HubHelpers.h"

int main(void) {
    initTreasureHubProcess();
    while (1) {
        promptForCommand();
    }

    return 0;
}
