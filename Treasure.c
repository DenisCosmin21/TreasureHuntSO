#include "Treasure.h"

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "Log.h"

static size_t getLineCountOfStorage(const int huntFd) {//Returns the line number from the storage
    struct stat st;
    if (fstat(huntFd, &st) == -1) {
        perror("Impossible to verify file");
        exit(1);
    }

    return st.st_size / sizeof(TreasureData);
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

static void jsonEncodeTreasure(const TreasureData treasure, char *json) {
    sprintf(json, "{\n  \"treasudeId\" : \"%lu\",\n   \"userName\" : \"%s\",\n    \"Coordinates\" : [\n      \"Latitude\" : \"%f\",\n        \"Longitude\" : \"%f\"\n    ],\n    \"clueText\" : \"%s\",\n    \"value\" : \"%d\"\n}", treasure.treasureId, treasure.userName, treasure.coordinates.latitude, treasure.coordinates.longitude, treasure.clueText, treasure.value);
}


static size_t getLastIdFromHunt(const int huntFd) {
    struct stat st;
    if (fstat(huntFd, &st) == -1) {
        perror("Error when checking treasure storage size");
        exit(1);
    }

    if (st.st_size == 0) { //If the file is emtpy we have no records, so we return 0
        return 0;
    }
    //Else we have to get the last id and return it
    if (lseek(huntFd, -((long)sizeof(TreasureData)), SEEK_END) == -1) {//We need to move the cursor to offset - size of treasureData structre based on end of file cursor position
        perror("Error when trying to get last id");
        exit(1);
    }

    size_t treasureId = 0;
    if (read(huntFd, &treasureId, sizeof(treasureId)) == -1) {
        perror("Error when reading treasure id");
        exit(1);
    }

    return treasureId;
}

static char *getHuntPathById(const char *huntId, char *huntPath) {
    //Hunt is of the form baseHuntName#huntId
    strcpy(huntPath, baseHuntName);
    strcat(huntPath, "#");
    strcat(huntPath, huntId);

    return huntPath;
}

static int existsHunt(const char *huntPath) {
    struct stat st;
    return stat(huntPath, &st) != -1; //Folder exists if result different then -1
}

static int openHuntTreasureStorage(const char *huntId) {
    char huntPath[100] = {0};
    getHuntPathById(huntId, huntPath);

    if (!existsHunt(huntPath)) {
        //If the hunt directory doesn't exist we need to create it
        if (mkdir(huntPath, 0777) == -1) {
            perror("Impossible to create hunt folder");
            exit(1);
        }
    }

    char treasurePath[100] = {0};

    strcpy(treasurePath, huntPath);
    strcat(treasurePath, "/");
    strcat(treasurePath, baseTreasureListStorage);
    const int huntFd = open(treasurePath, O_CREAT | O_APPEND | O_RDWR, S_IWUSR | S_IRUSR | S_IROTH);

    if (huntFd < 0) {
        perror("Impossible to open file for the treaure storage");
        exit(1);
    }

    return huntFd;
}

static void writeFieldToFile(const int huntFd, const void *treasureField, const size_t treasureFieldSize) { //Made adiacent function for writing to fiel for not using redundant code
    if (write(huntFd, treasureField, treasureFieldSize) == -1) {
        perror("Imposible to write to file");
        exit(-1);
    }
}

static void writeTreasureToFile(const int huntFd, const TreasureData treasure) {//Need to write to file each field to have padding if needed between fields so that we can use fseek
    writeFieldToFile(huntFd, &(treasure.treasureId), sizeof(treasure.treasureId));
    writeFieldToFile(huntFd, treasure.userName, sizeof(treasure.userName));
    writeFieldToFile(huntFd, &(treasure.coordinates), sizeof(treasure.coordinates));
    writeFieldToFile(huntFd, treasure.clueText, sizeof(treasure.clueText));
    writeFieldToFile(huntFd, &(treasure.value), sizeof(treasure.value));
}

static void logAddOperation(const char *huntId, const TreasureData treasure) { //Added specific log for add operation
    char logMessage[1024] = {0};
    char json[1024] = {0};
    jsonEncodeTreasure(treasure, json);
    //Use a json format for the data to print as becouse of it's global usage
    sprintf(logMessage, "Treasure with id %lu successfully added to Hunt with id : %s.\n %s", treasure.treasureId, huntId, json);

    char huntPath[100] = {0};
    getHuntPathById(huntId, huntPath);

    char logPath[100] = {0};
    strcpy(logPath, huntPath);
    strcat(logPath, "/");
    strcat(logPath, baseName);
    strcat(logPath, "-");
    strcat(logPath, huntId);

    logSuccess(logMessage, logPath);
}

void addTreasure(const char * huntId, TreasureData treasure) {
    int huntFd = openHuntTreasureStorage(huntId);

    treasure.treasureId = getLastIdFromHunt(huntFd) + 1;

    writeTreasureToFile(huntFd, treasure);

    logAddOperation(huntId, treasure);

    if (close(huntFd) == -1) {
        perror("Impossible to close file");
        exit(1);
    }
}

TreasureData askUserForInput(void) {
    TreasureData treasure;

    fgets(treasure.userName, sizeof(treasure.userName), stdin);
    treasure.userName[strlen(treasure.userName) - 1] = '\0';

    scanf("%f", &(treasure.coordinates.latitude));
    scanf("%f", &(treasure.coordinates.longitude));
    getchar();
    fgets(treasure.clueText, sizeof(treasure.clueText), stdin);

    treasure.clueText[strlen(treasure.clueText) - 1] = '\0';
    scanf("%d", &(treasure.value));

    return treasure;
}

static ssize_t findTreasurePosition(const int huntFd, const size_t treasureId, size_t left, size_t right) {
    if (treasureId == 0) {
        return -1; //We can't have inegative position
    }

    size_t treasureIdFromStorage = 0;

    ssize_t middle = left + (right - left) / 2;

    if (lseek(huntFd, middle * sizeof(TreasureData), SEEK_SET) == -1) {//Go to position of middle and multiply it by the size of our treasure data structure to go to the beggining of the line
        perror("Impossible to seek to treasure position");
        exit(1);
    }

    if (read(huntFd, &(treasureIdFromStorage), sizeof(treasureIdFromStorage)) == -1) {
        perror("Impossible to read from file");
        exit(1);
    }

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
    if (read(huntFd, &(treasure), sizeof(treasure)) == -1) {//We can read it as a whole becouse we already padded each field with 0 where needed
        perror("Impossible to read from file");
        exit(1);
    }
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

    if (lseek(huntFd, pos * sizeof(TreasureData), SEEK_SET) == -1) {
        perror("Impossible to seek to treasure position");
        exit(1);
    }

    treasure = readTreasureFromFile(huntFd);

    return treasure;
}

static void logGetTreasureOperation(const char * huntId, const TreasureData treasure) {
    char logMessage[1024] = {0};
    char json[1024] = {0};
    jsonEncodeTreasure(treasure, json);
    //Use a json format for the data to print as becouse of it's global usage
    sprintf(logMessage, "Get treasure operation return data : \n%s", json);

    char huntPath[100] = {0};
    getHuntPathById(huntId, huntPath);

    char logPath[100] = {0};
    strcpy(logPath, huntPath);
    strcat(logPath, "/");
    strcat(logPath, baseName);
    strcat(logPath, "-");
    strcat(logPath, huntId);

    logInfo(logMessage, logPath);
}

TreasureData getTreasureFromHunt(const char * huntId, const char * treasureId) {
    const int huntFd = openHuntTreasureStorage(huntId);

    TreasureData treasure = getTreasureFromStorageById(huntFd, strtol(treasureId, NULL, 10));

    if (treasure.treasureId == 0) {
        fprintf(stderr, "No treasure with specified id found.");
        exit(-1);
    }

    logGetTreasureOperation(huntId, treasure);

    if (close(huntFd) == -1) { //Close the file descriptor
        perror("Impossible to close file");
        exit(1);
    }

    viewTreasure(treasure);

    return treasure;
}

void listTreasuresFromHunt(const char * huntId) {
    const int huntFd = openHuntTreasureStorage(huntId);

    for (ssize_t i = 0;i < getLineCountOfStorage(huntFd); i++) {//We go line by line getting each treasure and showing it untill we reach the end of file wich we know by reaching the last line
        TreasureData treasure = readTreasureFromFile(huntFd);

        viewTreasure(treasure);
    }

    if (close(huntFd) == -1) { //Close the file descriptor
        perror("Impossible to close file");
        exit(1);
    }
}


