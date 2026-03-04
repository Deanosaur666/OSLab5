#include <stdio.h>
#include <stdlib.h>
#include "Dean_libFS.h"
#include <string.h>
#include <ncurses.h>
#include <menu.h>
#include <unistd.h>
#include <linux/limits.h>
#include <dirent.h>

void drawFiles(struct dirent ** files, int fileCount, int fileIndex, int row, int col) {
    clear();
    for(int i = 0; i < fileCount; i ++) {
        char item[17];
        struct dirent * f = files[i];
        if(i == fileIndex) {
            attron(A_STANDOUT);
            if(f->d_type == DT_DIR)
                attron(COLOR_PAIR(1) | A_REVERSE);
        }
        else {
            attroff(A_STANDOUT);
            if(f->d_type == DT_DIR)
                attron(COLOR_PAIR(1));
        }

        sprintf(item, "%-16s", f->d_name);
        mvprintw(row + i, col, "%s", item);

        attrset(0);
        move(row + fileIndex, col);
        
    }
}

int main() {

    // setup curses
    initscr();
    start_color();
    clear();
    noecho();
    cbreak();
    keypad(stdscr, true);

    init_pair(1, COLOR_RED, COLOR_BLACK);

    int c;

    // my path
    char cwd[PATH_MAX];
    getcwd(cwd, sizeof(cwd));
    

    // OPTIONS
    // create file
    // write to file
    // print file contents
    // delete file

    // files in my path
    struct dirent **files;
    int fileCount = scandir(cwd, &files, NULL, alphasort);

    int fileIndex = 2;

    drawFiles(files, fileCount, fileIndex, 1, 1);

    refresh();

    while((c = getch()) != 'q') {
        if(c == KEY_UP) {
            fileIndex = ((fileIndex - 1) + fileCount) % fileCount;
        }
        else if(c == KEY_DOWN) {
            fileIndex = (fileIndex + 1) % fileCount;
        }
        else if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            echo();
            move(15, 0);
            clrtoeol();
            char buff[255];
            scanw("%255s", (char *)&buff);
            clrtoeol();
            mvprintw(15, 0, "%s", buff);
            move(0, 0);
            noecho();
        }
        drawFiles(files, fileCount, fileIndex, 1, 1);
        refresh();
        
    }

    endwin();

    for(int i = 0; i < fileCount; i ++)
        free(files[i]);
    free(files);

    return 0;
}