#include "Dean_libFS.h"
#include <assert.h>

FSOpenFile * fstree = NULL;

int fstreeAddTo(FSOpenFile * tree, FSOpenFile * file) {
    int c = strcmp(file->name, tree->name);
    if(c == 0)
        return ERROR_OPEN; // error, can't add existing file
    else if(c < 0) {
        if(tree->leftChild == NULL) {
            tree->leftChild = file;
            return SUCCESS;
        }
        else {
            return fstreeAddTo(tree->leftChild, file);
        }
    }
    else {
        if(tree->rightChild == NULL) {
            tree->rightChild = file;
            return SUCCESS;
        }
        else {
            return fstreeAddTo(tree->rightChild, file);
        }
    }
}

int fstreeAdd(FSOpenFile * file) {
    if(fstree == NULL) {
        fstree = file;
        return SUCCESS;
    }
    else        
        return fstreeAddTo(fstree, file);
}

FSOpenFile * fstreeFind(FSOpenFile * fstree, char * name, FSOpenFile ** parent) {
    if(fstree == NULL)
        return NULL;
    int c = strcmp(name, fstree->name);
    if(c == 0)
        return fstree;
    else if(c < 0) {
        if(parent != NULL)
            *parent = fstree;
        return fstreeFind(fstree->leftChild, name, parent);
    }
    else {
        if(parent != NULL)
            *parent = fstree;
        return fstreeFind(fstree->rightChild, name, parent);
    }
}

int fstreeDelete(char * name) {
    FSOpenFile * parent;
    FSOpenFile * file = fstreeFind(fstree, name, &parent);
    if(file == NULL)
        return ERROR_NOT_OPEN;
    else {
        if(file == fstree)
            fstree = NULL;
        int parentside = 0;
        if (parent != NULL)
            parentside = parent->leftChild == file ? -1 : parent->rightChild == file ? 1 : 0;
        FSOpenFile * l = file->leftChild;
        FSOpenFile * r = file->rightChild;
        free(file);
        if(parentside < 0)
            parent->leftChild = NULL;
        else if(parentside > 0)
            parent->rightChild = NULL;
        
        if(l != NULL)
            fstreeAdd(l);
        if(r != NULL)
            fstreeAdd(r);
        
        return SUCCESS;
    }
}

// Create a new file. Errors: ERROR_BADNAME or ERROR_FAIL
int fileCreate(char * name) {
    char bad_chars[] = "/\\!@%^*~|";
    if(strcspn(name, bad_chars) < strlen(name))
        return ERROR_BADNAME;

    FILE * f = fopen(name, "w");
    if(f == NULL)
        return ERROR_FAIL;
    else {
        fclose(f);
        return SUCCESS;
    }
}
// Open an existing file. Errors: ERROR_OPEN or ERROR_DNE
int fileOpen(char * name) {
    if(fstreeFind(fstree, name, NULL) != NULL)
        return ERROR_OPEN;
    else {
        FILE * f = fopen(name, "r");
        if(f == NULL) {
            return ERROR_DNE;
        }
        else {
            fclose(f);
            FSOpenFile * file = calloc(1, sizeof(FSOpenFile));
            snprintf(file->name, MAX_FILENAME, "%s", name);
            fstreeAdd(file);
            return SUCCESS;
        }
        
    }
}
// Read data from a file. Errors: ERROR_NOT_OPEN or ERROR_DNE
int fileRead(char * name, char * buffer, int max) {
    // is the file "open"?
    if(fstreeFind(fstree, name, NULL) == NULL) {
        return ERROR_NOT_OPEN;
    }
    else {
        FILE * f = fopen(name, "r");
        if(f == NULL) {
            return ERROR_DNE;
        }
        else {
            fread(buffer, 1, max, f);
            fclose(f);
            return SUCCESS;
        }
    }
}

// Write text to file. Errors: ERROR_NOT_OPEN
int fileWrite(char * name, char * text) {
    // is the file "open"?
    if(fstreeFind(fstree, name, NULL) == NULL) {
        return ERROR_NOT_OPEN;
    }
    else {
        FILE * f = fopen(name, "w");
        fprintf(f, "%s", text);
        fclose(f);
        return SUCCESS;
    }
}

// Close an open file. Errors: ERROR_NOT_OPEN
int fileClose(char * name) {
    return fstreeDelete(name);
}

// Delete a file. Errors: ERROR_OPEN or ERROR_FAIL
int fileDelete(char * name) {
    // is the file "open"?
    if(fstreeFind(fstree, name, NULL) != NULL) {
        return ERROR_OPEN;
    }
    else {
        if(remove(name) == 0)
            return SUCCESS;
        else
            return ERROR_FAIL;
    }
}

int sprintOpenFilesFrom(FSOpenFile * tree, char * buffer, int buffsize, int * start) {
    if(tree == NULL)
        return 0;
    else if(*start > buffsize)
        return 0;
    else {
        int count = 0;
        count += sprintOpenFilesFrom(tree->leftChild, buffer, buffsize, start);
        char name[MAX_FILENAME+1];
        sprintf(name, "%s\n", tree->name);
        int len = strlen(name);
        snprintf(buffer + *start, buffsize - *start, "%s", name);
        *start += len;
        count += sprintOpenFilesFrom(tree->rightChild, buffer, buffsize, start);
        return 1;
    }
}

int sprintOpenFiles(char * buffer, int buffsize) {
    int start = 0;
    return sprintOpenFilesFrom(fstree, buffer, buffsize, &start);
}