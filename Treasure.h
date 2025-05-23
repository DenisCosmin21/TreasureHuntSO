//
// Created by debian on 4/3/25.
//

#ifndef TREASURE_H
#define TREASURE_H
#define baseHuntName "huntFolder"
#define baseTreasureListStorage "treasures"
#include <stdio.h>

typedef struct {
    float latitude;
    float longitude;
} Coords;

typedef struct {
    size_t treasureId;
    char userName[20];
    Coords coordinates;
    char clueText[200];
    int value;
} TreasureData;

typedef enum {
    ADD_TREASURE,
    REMOVE_TREASURE,
    VIEW_TREASURE
}FileOperationSpecifier;

void addTreasure(const char * huntId, TreasureData treasure);

TreasureData askUserForInput(void);

TreasureData getTreasureFromHunt(const char * huntId, const char * treasureId);

void listTreasuresFromHunt(const char * huntId);

void removeTreasureFromHunt(const char * huntId, const char *treasureId);

char *getHuntPathById(const char *huntId);

void removeHunt(const char *huntId);

void listHunts(void);

int existsHunt(const char *huntPath);

size_t getLineCountOfStorage(const int huntFd);

int openHuntTreasureStorage(const char *huntId, FileOperationSpecifier operation);

TreasureData readTreasureFromFile(const int huntFd);

char *getHuntIdByPath(char *path);
#endif //TREASURE_H
