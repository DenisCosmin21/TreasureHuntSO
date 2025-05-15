//
// Created by debian on 4/3/25.
//
#ifndef LOG_H
#define LOG_H
#define baseName "logged_hunt"

void logInfo(const char *message, const char *path);

void logError(const char *message, const char *path);

void logWarning(const char *message, const char *path);

void logSuccess(const char *message, const char *path);
#endif //LOG_H
