
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

static const int gbufx = GRIDX + 2;
static const int gbufy = GRIDY + 2;
static const int debug = 0;

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

void printgrid(cell **grid, int highlight, int act){
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
                mvprintw((offy+j)-5, offx+(i*2), "(%d) %d ", GRIDX*j+i, grid[GRIDX*j+i]->value);
            } else {
                int val = grid[GRIDX*j+i]->value;
                switch(val){
                    case -1:
                        attron(COLOR_PAIR(1));
                        mvprintw((offy+j)-5, offx+(i*2), "M ");
                        attroff(COLOR_PAIR(1));
                        break;
                    case 1:
                        attron(COLOR_PAIR(2));
                        mvprintw((offy+j)-5, offx+(i*2), "%d ", val);
                        attroff(COLOR_PAIR(2));
                        break;
                    case 2:
                        attron(COLOR_PAIR(3));
                        mvprintw((offy+j)-5, offx+(i*2), "%d ", val);
                        attroff(COLOR_PAIR(3));
                        break;
                    case 3:
                        attron(COLOR_PAIR(4));
                        mvprintw((offy+j)-5, offx+(i*2), "%d ", val);
                        attroff(COLOR_PAIR(4));
                        break;
                    case 4:
                        attron(COLOR_PAIR(5));
                        mvprintw((offy+j)-5, offx+(i*2), "%d ", val);
                        attroff(COLOR_PAIR(5));
                        break;
                    case 5:
                        attron(COLOR_PAIR(6));
                        mvprintw((offy+j)-5, offx+(i*2), "%d ", val);
                        attroff(COLOR_PAIR(6));
                        break;
                    default:
                        mvprintw((offy+j)-5, offx+(i*2), "%d ", val);
                }
            }
        }
    }
}

int main(void){
    srand(time(NULL));
    cell **grid = initgrid();
    creategrid(grid);
    addmines(grid);
    // ncurses
    initscr();
    clear();
    noecho();
    cbreak();
    curs_set(0);
    printgrid(grid, 11, 2);
    refresh();
    getch();
    endwin();
    return 0;
}
