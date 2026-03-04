/*

This program uses the ncurses library
Install ncurses with:
sudo apt install libncurses-dev

*/

#define _GNU_SOURCE // so that it doesn't complain about scandir and alphasort
// to ignore annoying truncation warnings
#pragma GCC diagnostic ignored "-Wformat-truncation"

#include <stdio.h>
#include <stdlib.h>
#include "Dean_libFS.h"
#include <string.h>
#include <ncurses.h>
#include <menu.h>
#include <unistd.h>
#include <linux/limits.h>
#include <dirent.h>

#define FILE_DISPLAY_LENGTH 64

#define ASCII_PRINTABLE(a) (a >= 32 && a <= 126)

#define KEY_ESCAPE 27

void drawFiles(struct dirent ** files, int fileCount, int fileIndex, int row, int col) {
    clear();
    for(int i = 0; i < fileCount; i ++) {
        char item[FILE_DISPLAY_LENGTH + 1];
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

        snprintf(item, FILE_DISPLAY_LENGTH, "%s", f->d_name);
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
    ESCDELAY = 10;

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

    while(true) {
        c = getch();
        // escape or alt
        if(c == KEY_ESCAPE) {
            nodelay(stdscr, true);
            int k = getch();
            // ESCAPE
            if(k == ERR) {
                nodelay(stdscr, false);
                break;
            }
            // ALT
            else {
                nodelay(stdscr, false);
            }
        }
        if(c == KEY_UP) {
            fileIndex = ((fileIndex - 1) + fileCount) % fileCount;
        }
        else if(c == KEY_DOWN) {
            fileIndex = (fileIndex + 1) % fileCount;
        }
        else if(ASCII_PRINTABLE(c)) {
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