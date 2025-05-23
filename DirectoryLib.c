#include "DirectoryLib.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>

void removeDirectory(const char *directoryName) {
    //First we have to open the directory to get it's content
    //If we ewnt to delete a directory we hav to delete recursively it's content too before doing so
    //We will have a pots order tree traversal becouse of the directory generalized tree structure
    //For our case we need to emove only main file's content, but will generalize it for further addings
    DIR *directory = openDirectory(directoryName);

    //Now we need to loop throught it's kids, and if one kid is directory we need to call the function recursively for it
    struct dirent *entry = NULL;
    //First we need to iterate through the first 2 files that each directory have
    while ((entry = readdir(directory)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {//Pass over the case where we have the alis for current dir or parrent dir
            continue;
        }
        //First we need to get the file type to see if it's directory or not
        struct stat st;

        //Since entry->d_name returns the relativ path to the current folder, we need to build up the full path to the next folder
        //We build it like this : directoryName/entry->d_name etc...
        //And directory name is the path
        char path[1024] = {0};

        strcpy(path, directoryName);
        strcat(path, "/");

        strcat(path, entry->d_name); //We need to compute the path for files too

        if (stat(path, &st) == -1) { //We use stat method to see the type of the file becouse the dirent struct can;t be for sure on each system
            perror("Error in stat");
            exit(2);
        }

        if (S_ISDIR(st.st_mode)) {
            //We have directory and need to call recursively
            removeDirectory(path);
            rmdir(path);
        }
        else {
            if (unlink(path) == -1) { //Only when we have directory we need to do something different, in other cases  we just delete it
                perror("Impossible to remove file");
                exit(1);
            }
        }

    }

    closedir(directory);
    if (rmdir(directoryName) == -1) {
        perror("Error in removing directory");
        exit(2);
    }
    //After removing the directory content we remove it too
}

int existsDirectory(const char *directoryName) {
    struct stat st;
    return stat(directoryName, &st) == 0; //Folder exists if return is 0
}

void createDirectory(const char *directoryName) {
    if (mkdir(directoryName, 0777) == -1) {
        perror("Error in making new directory");
        exit(2);
    }
}

DIR *openDirectory(const char *directoryName) {
    DIR *dir = opendir(directoryName);
    if (dir == NULL) {
        perror("Error in opening directory");
        exit(2);
    }
    return dir;
}

void closeDirectory(DIR *dir) {
    if (closedir(dir) == -1) {
        perror("Error in closing directory");
        exit(2);
    }
}

char *getEntryName(DIR *directory) {
    static DIR *dir = NULL;
    if (dir == NULL) {
        dir = directory;
    }
    else
        if (directory != NULL) {
            dir = directory;
        }

    struct dirent *entry = NULL;
    if ((entry = readdir(dir)) != NULL) {
        return entry->d_name;
    }
    return NULL;
}



