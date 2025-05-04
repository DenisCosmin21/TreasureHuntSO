#ifndef OPERATIONHELPERS_H
#define OPERATIONHELPERS_H

void readOperationInfoFromFile(int sig);

void writeOperationInfoInFile(const char *operation);

void notifyProcess(const int processPid,const int signal);
#endif //OPERATIONHELPERS_H
