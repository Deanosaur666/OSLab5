#include "Dean_libFS.h"
#include <assert.h>

FSOpenFile * fstree = NULL;

int fstreeAddTo(FSOpenFile * tree, FSOpenFile * file) {
    int c = strcmp(file->name, tree->name);
    if(c == 0)
        return ERROR; // error, can't add existing file
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
        return ERROR;
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

// Create a new file
int fileCreate(char * name) {
    char bad_chars[] = "/\\!@%^*~|";
    if(strcspn(name, bad_chars) < strlen(name))
        return ERROR;

    FILE * f = fopen(name, "w");
    if(f == NULL)
        return ERROR;
    else {
        fclose(f);
        return SUCCESS;
    }
}
// Open an existing file
int fileOpen(char * name) {
    if(fstreeFind(fstree, name, NULL) != NULL)
        return ERROR;
    else {
        FSOpenFile * file = calloc(1, sizeof(FSOpenFile));
        snprintf(file->name, MAX_FILENAME, "%s", name);
        fstreeAdd(file);
        return SUCCESS;
    }
}
// Read data from a file
int fileRead(char * name, char * buffer, int max) {
    // is the file "open"?
    if(fstreeFind(fstree, name, NULL) == NULL) {
        return ERROR;
    }
    else {
        FILE * f = fopen(name, "r");
        fread(buffer, 1, max, f);
        fclose(f);
        return SUCCESS;
    }
}

int fileWrite(char * name, char * text) {
    // is the file "open"?
    if(fstreeFind(fstree, name, NULL) == NULL) {
        return ERROR;
    }
    else {
        FILE * f = fopen(name, "w");
        fprintf(f, "%s", text);
        fclose(f);
        return SUCCESS;
    }
}

// Close an open file
int fileClose(char * name) {
    return fstreeDelete(name);
}

// Delete a file
int fileDelete(char * name) {
    // is the file "open"?
    if(fstreeFind(fstree, name, NULL) != NULL) {
        return ERROR;
    }
    else {
        if(remove(name) == 0)
            return SUCCESS;
        else
            return ERROR;
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