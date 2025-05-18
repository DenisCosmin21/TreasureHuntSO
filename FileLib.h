//
// Created by debian on 4/11/25.
//

#ifndef FILELIB_H
#define FILELIB_H

#include <unistd.h>
#include <time.h>

int openFile(const char *path, const char *mode);

void closeFile(const int fd);

ssize_t readFile(const int fd, void *buffer, size_t len);

void writeFile(const int fd, const void *data, size_t len);

void makeSymlink(const char *from, const char *to);

int existsFile(const char *path);

struct stat getFileStat(const char *path);

struct stat getFdStat(const int fd);

void seekCursor(const int fd, const size_t offset, const int whence);

size_t getFileSize(const int fd);

void truncateFile(const int fd, const __off_t size);

struct timespec getFileTime(const int fd);

char *getFileHumanReadableTime(const int fd);
#endif //FILELIB_H
