#ifndef DEAN_LIBFS_H
#define DEAN_LIBFS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME 128
#define SUCCESS 0
#define ERROR_NOT_OPEN 1
#define ERROR_OPEN 2
#define ERROR_FAIL 3
#define ERROR_BADNAME 4
#define ERROR_DNE 5



typedef struct FSOpenFile {
    struct FSOpenFile * leftChild;
    char name[MAX_FILENAME];
    struct FSOpenFile * rightChild;
} FSOpenFile;

// Create a new file. Errors: ERROR_BADNAME or ERROR_FAIL
int fileCreate(char * name);
// Open an existing file. Errors: ERROR_OPEN or ERROR_DNE
int fileOpen(char * name);
// Read data from a file. Errors: ERROR_NOT_OPEN or ERROR_DNE
int fileRead(char * name, char * buffer, int max);
// Write text to file. Errors: ERROR_NOT_OPEN
int fileWrite(char * name, char * text);
// Close an open file. Errors: ERROR_NOT_OPEN
int fileClose(char * name);
// Delete a file. Errors: ERROR_OPEN or ERROR_FAIL
int fileDelete(char * name);
int sprintOpenFiles(char * buffer, int buffsize);

#endif