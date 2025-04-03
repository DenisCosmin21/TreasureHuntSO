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

void addTreasure(const char *, TreasureData);

TreasureData askUserForInput(void);

#endif //TREASURE_H
