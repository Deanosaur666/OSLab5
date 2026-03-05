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

void drawOptions(char * headline, char ** options, int nOptions, int sel, int row, int col) {
    attron(COLOR_PAIR(1));
    mvprintw(row, col, "%s", headline);
    attroff(COLOR_PAIR(1));

    for(int i = 0; i < nOptions; i ++) {
        if(i == sel) {
            attron(A_STANDOUT);
        }
        else {
            attroff(A_STANDOUT);
        }

        mvprintw(row + i + 1, col, "%s", options[i]);

        attrset(0);
        move(row + sel + 1, col-1);
        
    }
}

void drawPrompt(char * prompt, int row, int col) {
    attron(A_DIM);
    move(row, col);
    clrtoeol();
    mvprintw(row, col, "%s", prompt);
    attroff(A_DIM);
}

// reads at most 255 characters
void promptInput(char * prompt, char * buffer, int row, int col) {
    drawPrompt(prompt, row, col);
    echo();
    move(row + 1, col);
    clrtoeol();
    scanw("%255s", buffer);
    noecho();
    move(row + 1, col);
    clrtoeol();
}

void drawOpenFiles(int row, int col) {
    attron(COLOR_PAIR(1));
    mvprintw(row, col, "%s", "OPEN FILES");
    attroff(COLOR_PAIR(1));

    char buffer[1024] = { 0 };
    sprintOpenFiles(buffer, 1024);
    int i = 0;
    int start = 0;
    int lineLen = 0;
    while(start < 1024 && (lineLen = strcspn(buffer + start, "\n")) < strcspn(buffer + start, "\0")) {
        char line[1024] = { 0 };
        strncpy(line, buffer + start, lineLen);
        start += lineLen + 1;
        mvprintw(row + i + 1, col, "%s", line);
        i ++;
    }
}

// Your introduction should include at least 3-4 sentences with 2 paragraphs.

char * myIntro =
"I'm Dean Yockey, a CWU student. This program was written for an assignment.\n"
"This program uses the ncurses library for fancier terminal nonsense. I wanted a menu that would look nice and respond instantly to arrow keys.\n"
"The number keys are a pain to reach. A menu based on just entering numbers feels so unwieldy and old-fashioned."
;

typedef enum OPTIONS {
    CREATE,
    OPEN,
    WRITETEXT,
    WRITEINTRO,
    READ,
    CLOSE,
    DELETE,
    EXIT,

} OPTIONS;

int main() {

    char * options[] = {
        "CREATE FILE",
        "OPEN FILE",
        "WRITE TEXT TO FILE",
        "WRITE INTRO TO FILE",
        "READ FROM FILE",
        "CLOSE FILE",
        "DELETE FILE",
        "EXIT",
    };

    int nOptions = 8;

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
    

    // options and stuff

    int sel = 0;
    int promptLine = 11;
    int openFileCol = 32;
    char * prompt = "Select an option with SPACE or ENTER";
    char input[256];
    drawOpenFiles(1, openFileCol);
    drawPrompt(prompt, promptLine, 1);
    drawOptions("MENU", options, nOptions, sel, 1, 1);

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
            sel = ((sel - 1) + nOptions) % nOptions;
        }
        else if(c == KEY_DOWN) {
            sel = (sel + 1) % nOptions;
        }
        else if(c == '\n' || c == ' ') {
            if(sel == CREATE) {
                promptInput("Enter name of file to create:", input, promptLine, 1);
                int status = fileCreate(input);
                if(status == SUCCESS)
                    prompt = "File created successfully.";
                else if(status == ERROR_BADNAME)
                    prompt = "Invalid filename.";
                else
                    prompt = "File could not be created.";
            }
            else if(sel == OPEN) {
                promptInput("Enter name of file to open:", input, promptLine, 1);
                int status = fileOpen(input);
                if(status == SUCCESS)
                    prompt = "File opened successfully.";
                else if(status == ERROR_OPEN)
                    prompt = "File is already open.";
                else if(status == ERROR_DNE)
                    prompt = "File does not exist.";
                else
                    prompt = "File could not be opened.";
            }
            else if(sel == WRITETEXT) {
                promptInput("Enter name of file to write to:", input, promptLine, 1);
                char text[1024];
                promptInput("Enter text:", text, promptLine, 1);
                int status = fileWrite(input, text);
                if(status == SUCCESS)
                    prompt = "Text written successfully.";
                else if(status == ERROR_NOT_OPEN)
                    prompt = "File is not open.";
                else
                    prompt = "Text could not be written.";
            }
            else if(sel == WRITEINTRO) {
                promptInput("Enter name of file to write my intro to:", input, promptLine, 1);
                int status = fileWrite(input, myIntro);
                if(status == SUCCESS)
                    prompt = "Intro written successfully.";
                else if(status == ERROR_NOT_OPEN)
                    prompt = "File is not open.";
                else
                    prompt = "Intro could not be written.";
            }
            else if(sel == READ) {
                promptInput("Enter name of file to read from:", input, promptLine, 1);
                char text[1024]= { 0 };
                int status = fileRead(input, text, 1024);
                if(status == ERROR_NOT_OPEN)
                    prompt = "File is not open.";
                else if(status == ERROR_DNE)
                    prompt = "File does not exist.";
                else if(status == SUCCESS) {
                    clear();
                    mvprintw(0, 0, "%s", text);
                    refresh();
                    getch();
                    clear();
                }
                else
                    prompt = "File could not be read.";
            }
            else if(sel == CLOSE) {
                promptInput("Enter name of file to close:", input, promptLine, 1);
                int status = fileClose(input);
                if(status == SUCCESS)
                    prompt = "File closed successfully.";
                else if(status == ERROR_NOT_OPEN)
                    prompt = "File is not open.";
                else
                    prompt = "File could not be closed.";
            }
            else if(sel == DELETE) {
                promptInput("Enter name of file to delete:", input, promptLine, 1);
                int status = fileDelete(input);
                if(status == SUCCESS)
                    prompt = "File deleted successfully.";
                else if(status == ERROR_OPEN)
                    prompt = "Cannot delete a file that is open.";
                else
                    prompt = "File could not be deleted.";
            }
            else if(sel == EXIT) {
                break;
            }
        }
        clear();
        drawOpenFiles(1, openFileCol);
        drawPrompt(prompt, promptLine, 1);
        drawOptions("MENU", options, nOptions, sel, 1, 1);
        refresh();
        
    }

    endwin();

    return 0;
}