//
// Created by debian on 4/19/25.
//

#ifndef TREASUREMONITOR_H
#define TREASUREMONITOR_H

extern int monitorStarted;
extern int monitorPid;

void startMonitor(void);

void stopMonitor(void);

void exitHub(void);

void viewTreasure(const char *huntId,const char *treasureId);

void listTreasure(const char *huntId);

void listHunts(void);

void executeOperation(const char *operation);

void promptForCommand(int sig);

void initTreasureHubProcess(void);
#endif //TREASUREMONITOR_H
