#include "Treasure.h"

#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "Log.h"

static void jsonEncodeTreasure(TreasureData treasure, char *json) {
    sprintf(json, "{\n  \"treasudeId\" : \"%lu\",\n   \"userName\" : \"%s\",\n    \"Coordinates\" : [\n      \"Latitude\" : \"%f\",\n        \"Longitude\" : \"%f\"\n    ],\n    \"clueText\" : \"%s\",\n    \"value\" : \"%d\"\n}", treasure.treasureId, treasure.userName, treasure.coordinates.latitude, treasure.coordinates.longitude, treasure.clueText, treasure.value);
}


static ssize_t getLastIdFromHunt(const char *huntPath) {
    const int huntFd = open(huntPath, O_RDONLY);
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

static void writeFieldToFile(int huntFd, const void *treasureField, const size_t treasureFieldSize) { //Made adiacent function for writing to fiel for not using redundant code
    if (write(huntFd, treasureField, treasureFieldSize) == -1) {
        perror("Imposible to write to file");
        exit(-1);
    }
}

static void writeTreasureToFile(int huntFd, const TreasureData treasure) {//Need to write to file each field to have padding if needed between fields so that we can use fseek
    writeFieldToFile(huntFd, &(treasure.treasureId), sizeof(treasure.treasureId));
    writeFieldToFile(huntFd, treasure.userName, sizeof(treasure.userName));
    writeFieldToFile(huntFd, &(treasure.coordinates), sizeof(treasure.coordinates));
    writeFieldToFile(huntFd, treasure.clueText, sizeof(treasure.clueText));
    writeFieldToFile(huntFd, &(treasure.value), sizeof(treasure.value));
}

void addTreasure(const char * huntId, TreasureData treasure) {
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

    const int huntFd = open(treasurePath, O_WRONLY | O_CREAT | O_APPEND, S_IWUSR | S_IRUSR | S_IROTH);

    if (huntFd < 0) {
        perror("Impossible to open file for the treaure storage");
        exit(1);
    }

    treasure.treasureId = getLastIdFromHunt(treasurePath) + 1;

    writeTreasureToFile(huntFd, treasure);

    char logMessage[1024] = {0};
    char json[1024] = {0};
    jsonEncodeTreasure(treasure, json);
    //Use a json format for the data to print as becouse of it's global usage
    sprintf(logMessage, "Treasure with id %lu successfully added to Hunt with id : %s.\n %s", treasure.treasureId, huntId, json);

    char logPath[100] = {0};
    strcpy(logPath, huntPath);
    strcat(logPath, "/");
    strcat(logPath, baseName);
    strcat(logPath, "-");
    strcat(logPath, huntId);

    logSuccess(logMessage, logPath);

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


