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

void addmines(cell **grid){
    /*
     * Bounds:
     * x from GRIDX+1 -> 2*GRIDX
     * y from GRIDX+1 -> (GRIDX*GRIDY)+1
    */
    int count = 0;
    FILE *fp;
    while(count < MINES){
        /*int x = rand_lim(GRIDX) + 1;
        int y = rand_lim(GRIDY);*/
        int coord = rand_lim((GRIDY*GRIDX)+GRIDX);
        if(coord > GRIDX){
            if(grid[coord]->value != -1){
                if(debug){
                    fp = fopen("log.txt", "a+");
                    fprintf(fp, "%d\n", coord);
                    fclose(fp);
                }
                grid[coord]->value = -1;
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

void printgrid(cell **grid){
    int i, j;
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    for(j = 1; j < GRIDY+1; j++){
        for(i = 1; i < GRIDX+1; i++){
            if(grid[GRIDX*j+i]->value == -1){
                attron(COLOR_PAIR(1));
                if(debug)
                    printw("(%d) M ", GRIDX*j+i);
                else
                    printw("M ");
                attroff(COLOR_PAIR(1));
            } else {
                if(debug)
                    printw("(%d) %d ", GRIDX*j+i, grid[GRIDX*j+i]->value);
                else
                    printw("%d ", grid[GRIDX*j+i]->value);
            }
        }
        printw("\n");
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
    printgrid(grid);
    refresh();
    getch();
    endwin();
    return 0;
}
