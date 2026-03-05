#ifndef DEAN_LIBFS_H
#define DEAN_LIBFS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME 128
#define SUCCESS 0
#define ERROR 1


typedef struct FSOpenFile {
    struct FSOpenFile * leftChild;
    char name[MAX_FILENAME];
    struct FSOpenFile * rightChild;
} FSOpenFile;

// Create a new file
int fileCreate(char * name);
// Open an existing file
int fileOpen(char * name);
// Read text from a file
int fileRead(char * name, char * buffer, int max);
// Write text to a file
int fileWrite(char * name, char * text);
// Close an open file
int fileClose(char * name);
// Delete a file
int fileDelete(char * name);
int sprintOpenFiles(char * buffer, int buffsize);

#endif