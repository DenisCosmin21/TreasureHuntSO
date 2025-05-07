#ifndef OPERATIONHELPERS_H
#define OPERATIONHELPERS_H

char (*readOperationInfoFromFile(void))[1024];

void writeOperationInfoInFile(const char *operation);

void notifyProcess(const int processPid,const int signal);
#endif //OPERATIONHELPERS_H
