#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FileLib.h"
#include "Treasure.h"
#define MAX_BUCKETS 10
//Becouse of the complexity of the data, and the need of fast computing of info we will use
//A hash table type of algorithm. We will generate a key for each string, and afterwards a key for the string key, becouse we don't want to
//fill too much memory
/*usage example :
0 : 2000 => {name : Andrei, value : 5}, 3000 => {name : Marius, value : 10}
1 :
2 :
...etc
 */

typedef struct {
    char userName[20];
    size_t value;
}node;

typedef struct {
    node data;
    size_t key;
}node2;

typedef struct {
    node2 *entries;
    size_t maxSize;
    size_t currentSize;
}hashNode;

typedef struct {
    hashNode lists[MAX_BUCKETS];
}hashTable;

void printNode(node node) {
    printf("{value : %lld, name : %s}", node.value, node.userName);
}

void printNode2(node2 node) {
    printf("{key : %lld => ", node.key);
    printNode(node.data);
    printf("}");
}

void printTable(hashTable *table) {
    for (size_t i = 0; i < MAX_BUCKETS; i++) {
        printf("%lld : ", i);
        for (size_t j = 0;j < table->lists[i].currentSize;j++) {
            printNode2(table->lists[i].entries[j]);
            printf(",");
        }
        printf("\n");
    }
}

hashTable *initHashTable(void) {
    hashTable *table = malloc(sizeof(hashTable));
    for (size_t i = 0; i < MAX_BUCKETS; i++) {
        table->lists[i].entries = malloc(sizeof(node)); //As default add one entry only
        table->lists[i].maxSize = 1;
        table->lists[i].currentSize = 0;
    }
    return  table;
}

hashTable *clearHashTable(hashTable *table) {
    for (size_t i = 0; i < MAX_BUCKETS; i++) {
        free(table->lists[i].entries);
    }
    free(table);
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

size_t getHashPosition(node2 secondLevelEntry) {
    return secondLevelEntry.key % MAX_BUCKETS;
}

void checkListMaxSizeExceded(hashTable *table, size_t key) {
    if (table->lists[key].currentSize >= table->lists[key].maxSize) {
        //Use a efficent allocation schema to not allocate each element in part for scenarios with huge amounts of data
        table->lists[key].maxSize = table->lists[key].currentSize > 4 ? table->lists[key].currentSize / 2 * 3 : table->lists[key].currentSize + 1;
        //If siz greater than 4 we divide current size by 2 then multiply with 3, to have a nice and slow grow up in size, not exponential
        //focused on both small and large data sizes
        table->lists[key].entries = realloc(table->lists[key].entries, sizeof(node2) * table->lists[key].maxSize);//Realloc storage if needed
    }
}

void addEntry(hashTable *table, node entry) {
    node2 secondLevelNode = {entry, getKey(entry.userName)};
    size_t hashPosition = getHashPosition(secondLevelNode);

    checkListMaxSizeExceded(table, hashPosition);

    table->lists[hashPosition].entries[table->lists[hashPosition].currentSize++] = secondLevelNode;
}

int existsEntry(hashTable *table, node entry, size_t *listKey, size_t *entryKey) { //Use listKey and entryKey only if you want keys returned too for further usage. If you only need to check it's existence you can put null
    size_t computedKey = getKey(entry.userName);

    size_t computedListKey = getHashPosition((node2){entry, computedKey}); //get both keys first

    if (table->lists[computedListKey].currentSize > 0) {
        //Loop through records to see what we have inside the specified bucket
        for (size_t i = 0;i < table->lists[computedListKey].currentSize; i++) {
            if (table->lists[computedListKey].entries[i].key == computedKey && strcmp(table->lists[computedListKey].entries[i].data.userName, entry.userName) == 0) { //Second condition is a safety measure for duplicate keys, wich can happen but really rare
                if (listKey != NULL) {//Save the keys if needed by the user
                    *listKey = computedListKey;
                }
                if (entryKey != NULL) {
                    *entryKey = computedKey;
                }
                return i;
            }
        }
    }

    return -1;
}

node getEntry(hashTable *table, size_t listKey, size_t listIndex) {
    return table->lists[listKey].entries[listIndex].data;
}

int updateEntry(hashTable *table, node newEntry){
    size_t listKey;
    int index;

    if ((index = existsEntry(table, newEntry, &listKey, NULL)) != -1) {
        //Record exists so update it
        node oldValue = getEntry(table, listKey, index);
        node newValue;
        newValue.value = oldValue.value + newEntry.value;
        strcpy(newValue.userName, oldValue.userName);
        table->lists[listKey].entries[index].data = newValue;
        return 1;
    }
    //If it doesn't return 0
    return 0;
}

void printUserValues(hashTable *table) {
    for (size_t i = 0; i < MAX_BUCKETS; i++) {
        for (size_t j = 0;j < table->lists[i].currentSize;j++) {
            printf("%s => %lld\n", table->lists[i].entries[j].data.userName, table->lists[i].entries[j].data.value);
        }
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
        printTable(table);
        printUserValues(table);

        table = clearHashTable(table);
    }
    return 0;
}
