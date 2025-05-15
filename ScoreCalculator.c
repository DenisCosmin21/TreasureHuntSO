#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FileLib.h"
#include "Treasure.h"
#include "AvlTree.h"
#define MAX_BUCKETS 10

typedef struct {
    avlTree entries[MAX_BUCKETS];
}hashTable;

//Becouse of the complexity of the data, and the need of fast computing of info we will use
//A hash table type of algorithm. We will generate a key for each string, and afterwards a key for the string key, becouse we don't want to
//fill too much memory
/*usage example :
0 : 2000 => {name : Andrei, value : 5}, 3000 => {name : Marius, value : 10}
1 :
2 :
...etc
 */

void printTable(hashTable *table) {
    for (size_t i = 0; i < MAX_BUCKETS; i++) {
        printf("%lld : ", i);
        printTree(table->entries[i]);
        printf("\n");
    }
}

hashTable *initHashTable(void) {
    hashTable *table = malloc(sizeof(hashTable));
    for (size_t i = 0; i < MAX_BUCKETS; i++) {
        table->entries[i] = createTree();
    }
    return  table;
}

hashTable *clearHashTable(hashTable *table) {
    if (table != NULL) {
        for (size_t i = 0; i < MAX_BUCKETS; i++) {
            table->entries[i] = destructTree(table->entries[i]);
        }
        free(table);
    }
    return NULL;
}

size_t getKey(char *value) {
    size_t hash = 5381;
    int c;
    while ((c = *(int*)value++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

void addEntry(hashTable *table, node entry) {
    node2 secondLevelNode = {entry, getKey(entry.userName)};
    size_t listKey = secondLevelNode.key % MAX_BUCKETS;

    table->entries[listKey] = addNode(table->entries[listKey], secondLevelNode);
}

int existsEntry(hashTable *table, node data, size_t *entryKey) { //Use listKey and entryKey only if you want keys returned too for further usage. If you only need to check it's existence you can put null
    size_t computedKey = getKey(data.userName);

    size_t tableKey = computedKey % MAX_BUCKETS;

    if (table->entries[tableKey] != NULL ) {
        if (existsNode(table, computedKey) == 1) {
            if (entryKey != NULL) {
                *entryKey = computedKey;
            }
            return 1;
        }
    }
    return 0;
}

node *getEntry(hashTable *table, size_t tableKey, size_t key) {
    return getNode(table->entries[tableKey], key);
}

int updateEntry(hashTable *table, node newData){
    size_t computedKey = getKey(newData.userName);
    size_t tableKey = computedKey % MAX_BUCKETS;

    node *oldData = getEntry(table, tableKey, computedKey);

    if (oldData != NULL) {//If record exists
        oldData->value += newData.value;
        return 1;
    }
    //If it doesn't return 0
    return 0;
}

void showNode(avlTree tree) {
    if (tree != NULL) {
        printf("%s => %lld\n", tree->data.data.userName, tree->data.data.value);
    }
}

void printUserValues(hashTable *table) {
    for (size_t i = 0; i < MAX_BUCKETS; i++) {
        preorder(table->entries[i], showNode);
    }
}


int main(int argc, char *argv[]){
    if (argc != 2) {
        fprintf(stderr, "Usage: ./calculate_score <huntFolder>\n");
        exit(-1);
    }
    else {
        if (!existsHunt(argv[1])) {
            fprintf(stderr, "No hunt with specified name found\n");
            exit(-1);
        }

        hashTable *table = initHashTable(); //Init the hash table

        char *huntId = getHuntIdByPath(argv[1]);
        printf("Printing users from hunt : %s\n", huntId);

        int huntFd = openHuntTreasureStorage(huntId, VIEW_TREASURE);
        size_t recordsCount = getLineCountOfStorage(huntFd);

        for (size_t i = 0;i < recordsCount; i++) {
            TreasureData record = readTreasureFromFile(huntFd);
            node newEntry;
            strcpy(newEntry.userName, record.userName);
            newEntry.value = record.value;
            if (updateEntry(table, newEntry) == 0) {
                addEntry(table, newEntry);
            }
        }

        closeFile(huntFd);
        //printTable(table);
        printUserValues(table);

        table = clearHashTable(table);
    }
    return 0;
}