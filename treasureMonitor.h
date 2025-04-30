//
// Created by debian on 4/19/25.
//

#ifndef TREASUREMONITOR_H
#define TREASUREMONITOR_H
#include <stddef.h>

extern int monitorPid;

void startMonitor(void);

void stopMonitor(void);

void exitHub(void);

void viewTreasure(char *huntId, char *treasureId);

void executeOperation(const char *operation);

void promptForCommand(void);

void initTreasureHubProcess(void);
#endif //TREASUREMONITOR_H
