#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "tetris.h"

piece_t *pieces[MAX_PIECES];
piece_t *curr_piece = NULL;
char error[32];

int main(){
    srand(time(NULL));
    init_curses();
    draw_well();
    pieces[0] = add_piece();
    curr_piece = pieces[0];

    while(1){
        if (getch() == 'q') {
            break;
        }
        if(curr_piece){
            update_curr_piece();

        }
        if(curr_piece){
            if (getch() == ' '){
                curr_piece->flipped ? (curr_piece->flipped = false) :
                    (curr_piece->flipped = true);
            }
        }
        usleep(100);
        clear_scr();
    }
    endwin();
    return 0;
}

void init_curses(){
    initscr();
    noecho();
    cbreak();
    timeout(100);
}

void draw_well(){
    int r, c;
    for (r = 0; r < HEIGHT; r++){
        mvprintw(r, 0, "#");
        mvprintw(r, WIDTH, "#");
    }
    for(c = 0; c < WIDTH+1; c++){
        mvprintw(HEIGHT, c, "#");
    }
}

piece_t *add_piece(){
    int i;
    piece_t *new_piece = malloc(sizeof(piece_t));
    if (!new_piece){
        sprintf(error, "malloc(), line %d file %s", __LINE__, __FILE__);
        perror(error);
        exit(1);
    }

    new_piece->r = 1;
    new_piece->c = 1;
    char str[5] = {"#"};
    for(i = 0; i < 4; i++){
        strcpy(new_piece->shape[i], str);
    }
    return new_piece;
}

void update_curr_piece(){
    int i;
    if (curr_piece->r > HEIGHT-5) {
        curr_piece = NULL;
        return;
    }
    curr_piece->r++;
    if (!curr_piece->flipped){
        for(i = 0; i < 4; i++){
            mvprintw(curr_piece->r+i, curr_piece->c, "#");
        }
    }
    else{
        mvprintw(curr_piece->r, curr_piece->c, "####");
    }
}

void clear_scr(void){
    int r;
    for(r = 1; r < HEIGHT; r++){
        mvprintw(r, 1, "             ");
    }
}
