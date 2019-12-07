#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include "tetris.h"

piece_t *pieces[MAX_PIECES];
piece_t *curr_piece = NULL;
char error[32];
int num_pieces = 0;

int main(){
    int ch;

    srand(time(NULL));
    init_curses();
    draw_well();
    pieces[0] = add_piece();
    curr_piece = pieces[0];

    while(1){
        ch = getch();
        if (ch == 'q'){
            break;
        }
        else if (ch == ' ' && curr_piece){
            curr_piece->do_flip ? (curr_piece->do_flip = false) :
                (curr_piece->do_flip = true);
        }
        if(curr_piece){
            update_curr_piece();
        }
        else{
            if (num_pieces++ > MAX_PIECES-1){
                break;
            }
            num_pieces += 1;
            curr_piece = add_piece();
            pieces[num_pieces] = curr_piece;
        }
        usleep(1000*100);
    }
    endwin();
    return 0;
}

void init_curses(){
    initscr();
    noecho();
    cbreak();
    curs_set(0);
    nodelay(stdscr, TRUE);
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
    new_piece->do_flip = false;
    new_piece->is_horizontal = false;
    char str[5] = {"#"};
    for(i = 0; i < 4; i++){
        strcpy(new_piece->shape[i], str);
    }
    return new_piece;
}

void update_curr_piece(){
    int idx;
    piece_t *p = curr_piece;
    if (!p->is_horizontal && p->r > HEIGHT-5) {
        curr_piece = NULL;
        return;
    }
    else if (p->is_horizontal && p->r > HEIGHT-2) {
        curr_piece = NULL;
        return;
    }
    if (!p->is_horizontal) {
        mvprintw(p->r, p->c, " ");
        p->r++;
        for(idx = 0; idx < 4; idx++){
            mvprintw(p->r+idx, p->c, "#");
        }
    }
    else{
        mvprintw(p->r, p->c, "    ");
        p->r++;
        mvprintw(p->r, p->c, "####");
    }
    if (p->do_flip){
        if(!p->is_horizontal){ //Vertical and do flip
            for(idx = 0; idx < 4; idx++){
                mvprintw(p->r+idx, p->c, " ");
            }
            mvprintw(p->r, p->c, "####");
            p->is_horizontal = true;
        }
        else{ //Horizontal and do flip
            //piece's height must be > HEIGHT-5 to do a vertical flip
            if (p->r < HEIGHT-5){
                mvprintw(p->r, p->c, "    ");
                for(idx = 0; idx < 4; idx++){
                    mvprintw(p->r+idx, p->c, "#");
                }
                p->is_horizontal = false;
            }
        }
        p->do_flip = false;
    }
}

