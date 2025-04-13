#include "Treasure.h"

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "Log.h"
#include "DirectoryLib.h"
#include "FileLib.h"
#include <ctype.h>

char huntPath[100] = {0}; //Store the huntPath globally for easier access to it, and for removing redundant code

char logPath[100] = {0};

static size_t getLineCountOfStorage(const int huntFd) {//Returns the line number from the storage

    return getFileSize(huntFd) / sizeof(TreasureData); //Only fixed size will be in the file, not any chance of having float data
}

static void printTreasureCoordinates(const Coords coordinates) {
    printf("Latitude : %f; Longitude : %f\n", coordinates.latitude, coordinates.longitude);
}

static void viewTreasure(const TreasureData treasureToView) {
    printf("-------------------------------------------------------\n");
    printf("treasureId : %ld\n", treasureToView.treasureId);
    printf("userName : %s\n", treasureToView.userName);
    printf("Coordinates : ");
    printTreasureCoordinates(treasureToView.coordinates);
    printf("clueText : %s\n", treasureToView.clueText);
    printf("value : %d\n", treasureToView.value);
}

static char *jsonEncodeTreasure(const TreasureData treasure) {

    static char json[1024] = "\0";

    sprintf(json, "{\n  \"treasudeId\" : \"%lu\",\n   \"userName\" : \"%s\",\n    \"Coordinates\" : [\n      \"Latitude\" : \"%f\",\n        \"Longitude\" : \"%f\"\n    ],\n    \"clueText\" : \"%s\",\n    \"value\" : \"%d\"\n}", treasure.treasureId, treasure.userName, treasure.coordinates.latitude, treasure.coordinates.longitude, treasure.clueText, treasure.value);

    return json;
}


static size_t getLastIdFromHunt(const int huntFd) {
    if (getFileSize(huntFd) == 0) { //If the file is emtpy we have no records, so we return 0
        return 0;
    }
    //Else we have to get the last id and return it
    seekCursor(huntFd, -((long)sizeof(TreasureData)), SEEK_END);
    //We need to move the cursor to offset - size of treasureData structre based on end of file cursor position


    size_t treasureId = 0;

    readFile(huntFd, &treasureId, sizeof(treasureId));

    return treasureId;
}

char *getHuntPathById(const char *huntId) {
    //Hunt is of the form baseHuntName#huntId
    strcpy(huntPath, baseHuntName);
    strcat(huntPath, "#");
    strcat(huntPath, huntId);
    //At the same time compute the logPath too for further usage

    strcpy(logPath, huntPath);
    strcat(logPath, "/");
    strcat(logPath, baseName);
    strcat(logPath, "-");
    strcat(logPath, huntId);

    return huntPath;
}

static int existsHunt(const char *huntPath) {
    return existsDirectory(huntPath);
}

static int openHuntTreasureStorage(const char *huntId, FileOperationSpecifier operation) {
    getHuntPathById(huntId);

    if (!existsHunt(huntPath)) {
        //If the hunt directory doesn't exist we need to create it
        createDirectory(huntPath);
    }

    char treasurePath[1024] = {0};

    strcpy(treasurePath, huntPath);
    strcat(treasurePath, "/");
    strcat(treasurePath, baseTreasureListStorage);

    int huntFd = 0;

    switch (operation) {
        case(ADD_TREASURE) :
            huntFd = openFile(treasurePath, "ra");
            break;
        case(REMOVE_TREASURE) :
            huntFd = openFile(treasurePath, "rw");
            break;
        default :
            break;
    }

    return huntFd;
}

static void writeFieldToFile(const int huntFd, const void *treasureField, const size_t treasureFieldSize) { //Made adiacent function for writing to fiel for not using redundant code
    writeFile(huntFd, treasureField, treasureFieldSize);
}

static void writeTreasureToFile(const int huntFd, const TreasureData treasure) {//Need to write to file each field to have padding if needed between fields so that we can use fseek
    writeFieldToFile(huntFd, &(treasure.treasureId), sizeof(treasure.treasureId));
    writeFieldToFile(huntFd, treasure.userName, sizeof(treasure.userName));
    writeFieldToFile(huntFd, &(treasure.coordinates), sizeof(treasure.coordinates));
    writeFieldToFile(huntFd, treasure.clueText, sizeof(treasure.clueText));
    writeFieldToFile(huntFd, &(treasure.value), sizeof(treasure.value));
}

void addTreasure(const char * huntId, TreasureData treasure) {
    int huntFd = openHuntTreasureStorage(huntId, ADD_TREASURE);

    treasure.treasureId = getLastIdFromHunt(huntFd) + 1;

    writeTreasureToFile(huntFd, treasure);

    char logMessage[1024] = {0};

    //Use a json format for the data to print as becouse of it's global usage
    sprintf(logMessage, "Treasure with id %lu successfully added to Hunt with id \"%s\".\n %s", treasure.treasureId, huntId, jsonEncodeTreasure(treasure));

    logSuccess(logMessage, logPath);

    closeFile(huntFd);
}

void readField(void *buffer, const size_t count, const int fieldType) //1 => char, 2 => int 3 => float
{
    size_t read = 0;
    char readBuffer[1024];

    while ((readBuffer[read] = getchar()) != '\n') {
        switch (fieldType) {
            case(1) :
                if (read == count) {
                    perror("Max cnt exceded");
                    exit(-1);
                }
                break;
            case(2) :
                if (!isdigit(readBuffer[read])) {
                    perror("Number field. No characters. Format : 123");
                    exit(-1);
                }
            break;
            case(3) :
                if (!isdigit(readBuffer[read]) && readBuffer[read] != '.') {
                    perror("Float field. Format : 1.27");
                    exit(-1);
                }
            break;
            default :
                break;
        }

        read++;
    }

    readBuffer[read] = 0;

    switch (fieldType) {
        case(1) :
            memcpy(buffer, readBuffer, count);
        break;
        case(2) :
            *(int *)buffer = strtol(readBuffer, NULL, 10);
        break;
        case(3) :
            *(float *)buffer = strtof(readBuffer, NULL);
    }
}

TreasureData askUserForInput(void) {
    TreasureData treasure;

    printf("Username : ");
    readField(treasure.userName, sizeof(treasure.userName), 1);
    printf("Latitude : ");
    readField(&(treasure.coordinates.latitude), 0, 3);
    printf("Longitude : ");
    readField(&(treasure.coordinates.longitude), 0, 3);
    printf("ClueText : ");
    readField(treasure.clueText, sizeof(treasure.clueText), 1);
    printf("Value : ");
    readField(&(treasure.value), 0, 2);
    return treasure;
}

static ssize_t findTreasurePosition(const int huntFd, const size_t treasureId, size_t left, size_t right) {
    if (treasureId == 0) {
        return -1; //We can't have inegative position
    }

    size_t treasureIdFromStorage = 0;

    ssize_t middle = left + (right - left) / 2;

    //Go to position of middle and multiply it by the size of our treasure data structure to go to the beggining of the line
    seekCursor(huntFd, middle * sizeof(TreasureData), SEEK_SET);

    readFile(huntFd, &(treasureIdFromStorage), sizeof(treasureIdFromStorage));

    if (treasureIdFromStorage == treasureId) {
        return middle;
    }

    if (left < right) {
        if (treasureId < treasureIdFromStorage) { //We search in left because our id is smaller the one we found
            return findTreasurePosition(huntFd, treasureId, left, middle);
        }

        return findTreasurePosition(huntFd, treasureId, middle + 1, right);
    }
    else
        return -1; //If the id is not inside we return -1 like a error becouse we can't have negative position
}

static TreasureData readTreasureFromFile(const int huntFd) {
    TreasureData treasure;
    //We can read it as a whole becouse we already padded each field with 0 where needed
    readFile(huntFd, &(treasure), sizeof(treasure));
    return treasure;
}

static TreasureData getTreasureFromStorageById(const int huntFd, const ssize_t treasureId) {
    //We can use an adapted method of binary search to search for specific index, becouse we will never insert a index smaller then the last index
    //First to get the number of records we can divide the total size by structure size
    TreasureData treasure;

    ssize_t pos = findTreasurePosition(huntFd, treasureId, 0, getLineCountOfStorage(huntFd));

    if (pos == -1) {
        return (TreasureData){0}; //We return a nulll treasure
    }

    seekCursor(huntFd, pos * sizeof(TreasureData), SEEK_SET);

    treasure = readTreasureFromFile(huntFd);

    return treasure;
}

TreasureData getTreasureFromHunt(const char * huntId, const char * treasureId) {

    char logMessage[1024] = {0};

    if (!existsHunt(getHuntPathById(huntId))) {
        //First check if the hunt exists
        sprintf(logMessage, "No hunt with id : %s found", huntId);
        logError(logMessage, logPath);
        exit(-1);
    }

    const int huntFd = openHuntTreasureStorage(huntId, ADD_TREASURE);

    TreasureData treasure = getTreasureFromStorageById(huntFd, strtol(treasureId, NULL, 10));

    if (treasure.treasureId == 0) {
        fprintf(stderr, "No treasure with specified id found.");
        sprintf(logMessage, "Treasure with id : %ld not found", treasure.treasureId);
        logError(logMessage, logPath);
        exit(-1);
    }

    //Use a json format for the data to print as becouse of it's global usage
    sprintf(logMessage, "Get treasure operation return data : \n%s", jsonEncodeTreasure(treasure));

    logSuccess(logMessage, logPath);

    closeFile(huntFd);

    viewTreasure(treasure);

    return treasure;
}

void listTreasuresFromHunt(const char * huntId) {
    char logMessage[1024] = {0};

    if (!existsHunt(getHuntPathById(huntId))) {
        //First check if the hunt exists
        sprintf(logMessage, "No hunt with id : %s found", huntId);
        logError(logMessage, NULL);
        perror("Hunt not found");
        
        exit(-1);
    }

    const int huntFd = openHuntTreasureStorage(huntId, ADD_TREASURE);

    printf("Hunt name : %s\n", huntId);

    printf("Hunt size : %lld\n", getFileSize(huntFd));

    printf("Last modification : %s\n", getFileHumanReadableTime(huntFd));

    for (ssize_t i = 0;i < getLineCountOfStorage(huntFd); i++) {//We go line by line getting each treasure and showing it untill we reach the end of file wich we know by reaching the last line
        TreasureData treasure = readTreasureFromFile(huntFd);

        viewTreasure(treasure);
    }

    closeFile(huntFd);
}

void removeTreasureFromHunt(const char * huntId, const char *treasureId) {

    char logMessage[1024] = {0};

    if (!existsHunt(getHuntPathById(huntId))) {
        //First check if the hunt exists
        sprintf(logMessage, "No hunt with id : %s found", huntId);
        logError(logMessage, NULL);
        perror("No hunt with specified id found");
        exit(-1);
    }

    const int huntFd = openHuntTreasureStorage(huntId, REMOVE_TREASURE);

    ssize_t treasurePosition = findTreasurePosition(huntFd, strtol(treasureId, NULL, 10), 0, getLineCountOfStorage(huntFd));

    if (treasurePosition == -1){//Check if the specified treasure exists
        sprintf(logMessage, "Treasure with id : %s not found in hunt : %s", treasureId, huntId);
        logError(logMessage, logPath);

        perror("Impossible to find treasure position");
        exit(-1);
    }

    //After getting the treasure position from the file move the pointer to specified position
    //first check if the treasure is the last one so that we won;t store data in buffer and we just delete it
    if (treasurePosition != getLineCountOfStorage(huntFd)) {
        char readBuffer[sizeof(TreasureData)]; //use the buffer to store data from the file so that i can shift the dafa. In buffer i will svae the next record and so on
        //As the buffer we can use the TreasureData type too but we do this for generalization ,and better understanding that we get bytes from the file
        for (;treasurePosition < getLineCountOfStorage(huntFd) - 1;treasurePosition++) {

            //We move the pointer to the next record
            seekCursor(huntFd, (treasurePosition + 1) * sizeof(TreasureData), SEEK_SET);

            //Read the data to buffer
            readFile(huntFd, readBuffer, sizeof(TreasureData));

            //After reading we have to move the cursor to the point where we want to insert the buffer
            seekCursor(huntFd, treasurePosition * sizeof(TreasureData), SEEK_SET);

            writeFile(huntFd, readBuffer, sizeof(TreasureData));
        }
    }

    truncateFile(huntFd, (getLineCountOfStorage(huntFd) - 1) * sizeof(TreasureData));

    sprintf(logMessage, "Removing treasure with id %s from hunt %s", treasureId, huntId);

    logSuccess(logMessage, logPath);

    listTreasuresFromHunt(huntId);//After that we show it to see changes

    closeFile(huntFd);
}

void removeHunt(const char *huntId) {
    //First build the treasure folder name from the id
    char huntPath[100] = {0};
    char logMessage[1024] = {0};

    getHuntPathById(huntId);

    if (existsHunt(huntPath)) {//Check if hunt exists first
        removeDirectory(huntPath);
        sprintf(logMessage, "Removing hunt with id %s", huntId);
        logSuccess(logMessage, NULL); //Becouse we remove the full hunt directory we need to log data in the main log file instead of the symbolik linked file
        return;
    }

    sprintf(logMessage, "Hunt with id %s not found.", huntId);

    logError(logMessage, NULL);
}
