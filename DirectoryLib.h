//
// Created by debian on 4/4/25.
//

#ifndef DIRECTORYLIB_H
#define DIRECTORYLIB_H
#include <dirent.h>

void removeDirectory(const char *directoryName);

int existsDirectory(const char *directoryName);

void createDirectory(const char *directoryName);

DIR *openDirectory(const char *directoryName);
#endif //DIRECTORYLIB_H
