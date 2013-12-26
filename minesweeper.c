
/*
 * Great ncurses resource
 * http://www.tldp.org/HOWTO/NCURSES-Programming-HOWTO/index.html
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#define GRIDX 10
#define GRIDY 10
#define MINES 8

/* grid constants */
static const int gbufx = GRIDX + 2;
static const int gbufy = GRIDY + 2;
static const int debug = 0;

/* runtime vars */
static int playing = 1;
/*
 * 1 - failure
 * 2 - complete
*/
static int exitmsg = 0;

typedef struct {
    int value, hidden, marked;
} cell;

int rand_lim(int limit){
    /* 0 - limit inclusive */
    int div = RAND_MAX / (limit+1);
    int ret;
    do {
        ret = rand() / div;
    } while(ret > limit);
    return ret;
}

cell **initgrid(){
    cell **grid = malloc(sizeof(cell*)*gbufx*gbufy);
    return grid;
}

void creategrid(cell **grid){
    int i, j;
    for(j = 0; j < gbufy; j++){
        for(i = 0; i < gbufx; i++){
            grid[gbufx*j+i] = malloc(sizeof(cell));
            grid[gbufx*j+i]->value  = 0;
            grid[gbufx*j+i]->hidden = 1;
            grid[gbufx*j+i]->marked = 0;
        }
    }
}

void countmines(cell **grid, int loc){
    if(grid[loc - (GRIDX + 1)]->value != -1) grid[loc - (GRIDX + 1)]->value += 1;
    if(grid[loc - (GRIDX)]->value     != -1) grid[loc - (GRIDX)]->value     += 1;
    if(grid[loc - (GRIDX - 1)]->value != -1) grid[loc - (GRIDX - 1)]->value += 1;
    if(grid[loc - 1]->value           != -1) grid[loc - 1]->value           += 1;
    if(grid[loc + 1]->value           != -1) grid[loc + 1]->value           += 1;
    if(grid[loc + (GRIDX + 1)]->value != -1) grid[loc + (GRIDX + 1)]->value += 1;
    if(grid[loc + (GRIDX)]->value     != -1) grid[loc + (GRIDX)]->value     += 1;
    if(grid[loc + (GRIDX - 1)]->value != -1) grid[loc + (GRIDX - 1)]->value += 1;
    /* no wrap */
    if((loc-1)%GRIDX == 0){
        if(grid[loc-(GRIDX+1)]->value > 0) grid[loc-(GRIDX+1)]->value -= 1;
        if(grid[loc- 1]->value        > 0) grid[loc - 1]->value       -= 1;
        if(grid[loc+(GRIDX-1)]->value > 0) grid[loc+(GRIDX-1)]->value -= 1;
    }
    if(loc % GRIDX == 0){
        if(grid[loc-(GRIDX-1)]->value > 0) grid[loc-(GRIDX-1)]->value -= 1;
        if(grid[loc + 1]->value       > 0) grid[loc + 1]->value       -= 1;
        if(grid[loc+(GRIDX+1)]->value > 0) grid[loc+(GRIDX+1)]->value -= 1;
    }
}

void addmines(cell **grid){
    /*
     * Bounds:
     * x from GRIDX+1 -> 2*GRIDX
     * y from GRIDX+1 -> (GRIDX*GRIDY)+1
    */
    int count = 0;
    FILE *fp;
    while(count < MINES){
        int coord = rand_lim((GRIDY*GRIDX)+GRIDX);
        if(coord > GRIDX){
            if(grid[coord]->value != -1){
                if(debug){
                    fp = fopen("log.txt", "a+");
                    fprintf(fp, "%d\n", coord);
                    fclose(fp);
                }
                grid[coord]->value = -1;
                countmines(grid, coord);
                count++;
            }
        }
    }
    if(debug){
        fp = fopen("log.txt", "a+");
        fprintf(fp, "------\n");
        fclose(fp);
    }
}

void checkmines(cell **grid){
    int i, j;
    int count;
    for(j = 1; j < GRIDY+1; j++){
        for(i = 1; i < GRIDX+1; i++){
            int v = GRIDX*j+i;
            if(grid[v]->value == -1 && grid[v]->marked == 1){
                count++;
            }
        }
    }
    if(count == MINES){
        playing = 0;
        exitmsg = 2;
    }
}

void printgrid(cell **grid, int highlight, int act){
    /*
     * act:
     * 1 - reveal
     * 2 - flag
     * 3 - potential
    */
    if(act == 1){
        // can't reveal marked cells
        if(grid[highlight]->marked != 1)
            grid[highlight]->hidden = 0;
        if(grid[highlight]->value == -1){
            exitmsg = 1;
            playing = 0;
        }
    }
    if(act == 2){
        // unmark cell if its marked
        if(grid[highlight]->marked == 1)
            grid[highlight]->marked = 0;
        else
            grid[highlight]->marked = 1;
    }
    int i, j;
    int row, col;
    getmaxyx(stdscr, row, col);
    int offx = (col - 20) / 2;
    int offy = row / 2;
    start_color();
    /* colours */
    init_pair(1, COLOR_RED,     COLOR_BLACK); /* failure */
    init_pair(2, COLOR_GREEN,   COLOR_BLACK); /* 1s */
    init_pair(3, COLOR_YELLOW,  COLOR_BLACK); /* 2s */
    init_pair(4, COLOR_BLUE,    COLOR_BLACK); /* 3s */
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK); /* 4s */
    init_pair(6, COLOR_CYAN,    COLOR_BLACK); /* 5s */
    for(j = 1; j < GRIDY+1; j++){
        for(i = 1; i < GRIDX+1; i++){
            if(debug){
                mvprintw((offy+j)-5, offx+(i*2), "%d", grid[GRIDX*j+i]->value);
            } else if(grid[GRIDX*j+i]->hidden == 1 && grid[GRIDX*j+i]->marked == 0){
                // for hidden cells
                if(highlight == GRIDX*j+i){
                    // highlight this square
                    wattron(stdscr, A_REVERSE);
                    mvprintw((offy+j)-5, offx+(i*2), "-");
                    wattroff(stdscr, A_REVERSE);
                } else {
                    mvprintw((offy+j)-5, offx+(i*2), "-");
                }
            } else if(grid[GRIDX*j+i]->marked == 1){
                // for marked cells
                if(highlight == GRIDX*j+i){
                    wattron(stdscr, A_REVERSE);
                    mvprintw((offy+j)-5, offx+(i*2), "F");
                    wattroff(stdscr, A_REVERSE);
                } else {
                    mvprintw((offy+j)-5, offx+(i*2), "F");
                }
            } else {
                int val = grid[GRIDX*j+i]->value;
                if(highlight == GRIDX*j+i){
                    // highlight this square
                    wattron(stdscr, A_REVERSE);
                    if(val == -1)
                        mvprintw((offy+j)-5, offx+(i*2), "M", val);
                    else
                        mvprintw((offy+j)-5, offx+(i*2), "%d", val);
                    wattroff(stdscr, A_REVERSE);
                } else {
                    switch(val){
                        case -1:
                            attron(COLOR_PAIR(1));
                            mvprintw((offy+j)-5, offx+(i*2), "M");
                            attroff(COLOR_PAIR(1));
                            break;
                        case 1:
                            attron(COLOR_PAIR(2));
                            mvprintw((offy+j)-5, offx+(i*2), "%d", val);
                            attroff(COLOR_PAIR(2));
                            break;
                        case 2:
                            attron(COLOR_PAIR(3));
                            mvprintw((offy+j)-5, offx+(i*2), "%d", val);
                            attroff(COLOR_PAIR(3));
                            break;
                        case 3:
                            attron(COLOR_PAIR(4));
                            mvprintw((offy+j)-5, offx+(i*2), "%d", val);
                            attroff(COLOR_PAIR(4));
                            break;
                        case 4:
                            attron(COLOR_PAIR(5));
                            mvprintw((offy+j)-5, offx+(i*2), "%d", val);
                            attroff(COLOR_PAIR(5));
                            break;
                        case 5:
                            attron(COLOR_PAIR(6));
                            mvprintw((offy+j)-5, offx+(i*2), "%d", val);
                            attroff(COLOR_PAIR(6));
                            break;
                        default:
                            mvprintw((offy+j)-5, offx+(i*2), "%d", val);
                    }
                }
            }
        }
    }
    mvprintw(row-2, 0, "curr: %d", highlight);
    refresh();
}

void printexit(){
    if(playing == 0){
        if(exitmsg == 1){
            // lost
            printw("You lose, sorry...");
        } else if(exitmsg == 2){
            // won
            printw("Congratulations, you won!");
        }
    }
    refresh();
}

int main(void){
    srand(time(NULL));
    cell **grid = initgrid();
    creategrid(grid);
    addmines(grid);
    // ncurses vars
    int curr = 11;
    int act  = 0;
    int c;
    // ncurses start
    initscr();
    clear();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);
    printgrid(grid, curr, act);
    refresh();
    while(playing == 1){
        act = 0;
        c   = wgetch(stdscr);
        switch(c){
            case KEY_DOWN:
                if(curr > GRIDX*GRIDY)
                    curr = curr % ((GRIDX*GRIDY)-GRIDX);
                else
                    curr += GRIDX;
                break;
            case KEY_UP:
                if(curr <= 2*GRIDX)
                    curr = (GRIDY*GRIDX)+(curr - GRIDX);
                else
                    curr -= GRIDX;
                break;
            case KEY_RIGHT:
                if((curr)%10 == 0)
                    curr = curr - (GRIDX-1);
                else
                    curr += 1;
                break;
            case KEY_LEFT:
                if((curr-1)%10 == 0)
                    curr = curr + (GRIDX-1);
                else
                    curr -= 1;
                break;
            case 114:
                act = 1;
                break;
            case 102:
                act = 2;
                break;
        }
        clear();
        printgrid(grid, curr, act);
        checkmines(grid);
    }
    clear();
    printexit();
    getch();
    endwin();
    return 0;
}
