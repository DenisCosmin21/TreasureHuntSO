#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "Log.h"
#include "Treasure.h"
#include "DirectoryLib.h"

#define basicPath "treasureHunt"

void parseOperation(const char * operation, const char **parameters, const size_t parametersCount) {
    if (strcmp(operation, "--add") == 0) {
        if (parametersCount != 3) {
            fprintf(stderr, "Wrong number of arguments\n");
            printf("Usage : add <hunt_id>\n");
            exit(-1);
        }

        TreasureData treasureToAdd = askUserForInput();
        addTreasure(parameters[0], treasureToAdd);
    }
    else if (strcmp(operation, "--list") == 0) {
        if (parametersCount != 3) {
            fprintf(stderr, "Wrong number of arguments\n");
            printf("Usage : list <hunt_id>\n");
            exit(-1);
        }

        listTreasuresFromHunt(parameters[0]);
    }
    else if (strcmp(operation, "--view") == 0) {
        if (parametersCount != 4) {
            fprintf(stderr, "Wrong number of arguments\n");
            printf("Usage : view <hunt_id> <id>\n");
            exit(-1);
        }

        TreasureData treasureReturned = getTreasureFromHunt(parameters[0], parameters[1]);
    }
    else if (strcmp(operation, "--remove_treasure") == 0) {
        if (parametersCount != 4) {
            fprintf(stderr, "Wrong number of arguments\n");
            printf("Usage : remove_treasure <hunt_id> <id>\n");
            exit(-1);
        }

        removeTreasureFromHunt(parameters[0], parameters[1]);
    }
    else if (strcmp(operation, "--remove_hunt") == 0) {
        if (parametersCount != 3) {
            fprintf(stderr, "Wrong number of arguments\n");
            printf("Usage : remove_hunt <hunt_id>\n");
            exit(-1);
        }

        removeHunt(parameters[0]);
    }
    else if (strcmp(operation, "--list_hunts") == 0) {
        if (parametersCount != 2) {
            fprintf(stderr, "Wrong number of arguments\n");
            printf("Usage : list_hunts\n");
            exit(-1);
        }

        listHunts();
    }
    else {
        fprintf(stderr, "Invalid operation\n");
        printf("Usage : \nadd <hunt_id>\nlist <hunt_id>\nview <hunt_id> <id>\nremove_treasure <hunt_id> <id>\nremove_hunt <hunt_id>");
        exit(-1);
    }
}

int main(const int argc, const char * argv[]) {
    if (argc < 1) {
        fprintf(stderr, "Wrong number of arguments\n");
        printf("Usage : \nadd <hunt_id>\nlist <hunt_id>\nview <hunt_id> <id>\nremove_treasure <hunt_id> <id>\nremove_hunt <hunt_id>");
        exit(-1);
    }


    parseOperation(argv[1], argv + 2, argc);

    return 0;
}