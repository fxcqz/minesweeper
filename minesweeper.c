#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#define GRIDX 10
#define GRIDY 10
#define MINES 8

static const int gbufx = GRIDX + 2;
static const int gbufy = GRIDY + 2;
static const int debug = 1;

typedef struct {
    int value, hidden, marked;
} cell;

int rand_lim(int limit){
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

void addmines(cell **grid){
    /*
     * Bounds:
     * x from GRIDX+1 -> 2*GRIDX
     * y from GRIDX+1 ->
    */
    int count = 0;
    while(count < MINES){
        int x = rand_lim(GRIDX);
        int y = rand_lim(GRIDY);
        if(grid[GRIDX*y+x]->value != -1
        && GRIDX*y+x >= GRIDX
        && GRIDX*y+x <= (GRIDX*GRIDY)+GRIDX){
            grid[GRIDX*y+x]->value = -1;
            count++;
        }
    }
}

void printgrid(cell **grid){
    int i, j;
    for(j = 0; j < gbufy; j++){
        for(i = 0; i < gbufx; i++){
            if(grid[GRIDX*j+i]->value == -1){
                start_color();
                init_pair(1, COLOR_RED, COLOR_BLACK);
                attron(COLOR_PAIR(1));
                printw("(%d) %d ", GRIDX*j+i, grid[GRIDX*j+i]->value);
                attroff(COLOR_PAIR(1));
            } else {
                printw("(%d) %d ", GRIDX*j+i, grid[GRIDX*j+i]->value);
            }
        }
        printw("\n");
    }
}

int main(void){
    srand(time(NULL));
    cell **grid = initgrid();
    creategrid(grid);
    //addmines(grid);
    // ncurses
    initscr();
    clear();
    noecho();
    cbreak();
    curs_set(0);
    printgrid(grid);
    refresh();
    getch();
    endwin();
    return 0;
}
