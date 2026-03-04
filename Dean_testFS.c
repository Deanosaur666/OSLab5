#include <stdio.h>
#include "Dean_libFS.h"
#include <string.h>
#include <ncurses.h>
#include <unistd.h>
#include <linux/limits.h>
#include <dirent.h>

#define C_BLACK 30
#define C_RED 31
#define C_GREEN 32
#define C_YELLOW 33
#define C_BLUE 34
#define C_MAGENTA 35
#define C_CYAN 36
#define C_WHITE 37

#define C_BG(c) (c + 10)
#define C_BRIGHT(c) (c + 60)

void cursorToHome() {
    // Write the sequence for clearing the display:
    // \x1B[2J - Clears the visible window
    // \x1B[3J - Clears the scroll back
    // \x1B[1;1H - Move cursor back to its home coordinates (top left)
    fputs("\x1B[3J\x1B[1;1H", stdout);  
    fflush(stdout);
}

void setColor(int c) {
    printf("\x1B[%dm", c);
}

int getFiles(char * names[255], char * types, int count, int offset) {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    int i = 0;
    if(d) {
        while((dir = readdir(d)) != NULL) {
            if(offset > 0)
                offset --;
            else if(dir->d_type == DT_DIR || 
                    dir->d_type == DT_REG) {
                names[i] = dir->d_name;
                types[i] = dir->d_type;
                // type could be DT_DIR for directory
                // or DT_REG for regular
                i ++;
            }
        }
        names[i] = "";
        types[i] = '\0';
    }

    return i;
}

int main() {

    // OPTIONS
    // create file
    // write to file
    // print file contents
    // delete file

    char * options[] = {
        "CREATE",
        "DISPLAY",
        "EDIT",
        "DELETE",
    };

    //char cwd[PATH_MAX];
    //getcwd(cwd, sizeof(cwd));

    initscr();
    clear();
    noecho();
    cbreak();
    keypad(stdscr, true);

    refresh();
    cursorToHome();
    setColor(C_BG(C_BRIGHT(C_RED)));
    printf("Hey hey hey.");
    setColor(0);
    printf("\nHey hey hey.\n");

    while(1) {
        int key = getch();
        if(key == KEY_UP) {
            printf("UP!\n");
            cursorToHome();
        }
        else if(key == KEY_DOWN) {
            printf("DOWN!\n");
            cursorToHome();
        }
        else if(key == 'q') {
            break;
        }
        setColor(C_BG(C_BRIGHT(C_RED)));
        printf("Hey hey hey.");
        setColor(0);
        printf("\nHey hey hey.\n");
        refresh();
        cursorToHome();
    }

    endwin();

    return 0;
}