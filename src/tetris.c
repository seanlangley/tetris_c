#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "tetris.h"

piece_t *curr_piece = NULL;
char error[32], buffer[32];
unsigned int score = 0, level = 1;
bool game_over = false;

int main(){
    int ch;

    srand(time(NULL));
    init_curses();
    draw_well();
    curr_piece = add_piece();
    while(1){
        sprintf(buffer, "Score: %d", score);
        mvprintw(2, 18, buffer);
        sprintf(buffer, "Level: %d", level);
        mvprintw(3, 18, buffer);
        ch = getch();
        if (ch == 'q'){
            break;
        }
        if (curr_piece){
            switch(ch){
                case 'a': move_horizontally(LEFT);    break;
                case 'd': move_horizontally(RIGHT);   break;
                case ' ': curr_piece->do_flip = true; break;
                default: break;
            }
            update_curr_piece();
        }
        else{
            curr_piece = add_piece();
        }
        if (game_over){
            break;
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
    timeout(50);
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
    piece_t *new_piece;
    
    new_piece = malloc(sizeof(piece_t));
    if (!new_piece){
        sprintf(error, "malloc(), line %d file %s", __LINE__, __FILE__);
        perror(error);
        exit(1);
    }

    new_piece->type = (enum piece_type)(rand() % 4);
    switch (new_piece->type){
        case I:
            if (mvinch(4, WIDTH/2) == '#'){
                game_over = true;
            }
            break;
        case SQUARE:
            if (mvinch(3, WIDTH/2) == '#' || mvinch(3, WIDTH/2+1) == '#'){
                game_over = true;
            }
            break;
        case J:
            if (mvinch(3, WIDTH/2-1) == '#' || mvinch(3, WIDTH/2) == '#'){
                game_over = true;
            }
            break;
        case L:
            if (mvinch(3, WIDTH/2) == '#' || mvinch(3, WIDTH/2+1) == '#'){
                game_over = true;
            }
            break;
    }
    if (game_over){
        free(new_piece);
        return NULL;
    }
    new_piece->r = 1;
    new_piece->c = WIDTH/2;
    new_piece->do_flip = false;
    new_piece->is_horizontal = false;
    return new_piece;
}

void update_curr_piece(){
    piece_t *p = curr_piece;
    bool make_new_piece = false;

    switch (p->type){
        case I:
            if ((!p->is_horizontal && mvinch(p->r+4, p->c) != ' ') ||
                    (p->is_horizontal && mvinch(p->r+1, p->c) != ' '))  {
                make_new_piece = true;
            }
            break;
        case SQUARE:
            if (mvinch(p->r+2, p->c) == '#'){
                make_new_piece = true;
            }
            break;
        case J:
            if (p->is_horizontal){
                if (mvinch(p->r+1, p->c) == '#' || mvinch(p->r+1, p->c+1) == '#' ||
                    mvinch(p->r+2, p->c+2) == '#'){
                    make_new_piece = true;
                }
            }
            else if (mvinch(p->r-1, p->c+3) == '#' ||
                     mvinch(p->r, p->c+3) == '#'){
                make_new_piece = true;
            }
        case L://TODO
            if (p->is_horizontal){
            }
            else{

            }
            break;
        default:
            break;
    }
    if (make_new_piece){
        free(curr_piece);
        curr_piece = NULL;
        clear_lines();
        return;
    }
    clear_piece(p);
    p->r++;
    if (p->do_flip){
        if (!p->is_horizontal && p->c < WIDTH-4){
            p->is_horizontal = true;
        }
        else if(p->is_horizontal){
            p->is_horizontal = false;
        }
        p->do_flip = false;
    }
    draw_piece(p);
}

void move_horizontally(direction_t direction){
    piece_t *p = curr_piece;
    int inc = 0;

    switch (p->type){
        case I:
            if (direction == LEFT && p->c > 1) {
                inc = -1;
            }

            else if (direction == RIGHT) {
                if ((p->is_horizontal && p->c < WIDTH-4) ||
                        (!p->is_horizontal && p->c < WIDTH-1)){
                    inc = 1;
                }
            }
            break;
        case SQUARE:
            if (direction == LEFT && mvinch(p->r, p->c-1) != '#' ){
                inc = -1;
            }
            else if (direction == RIGHT && mvinch(p->r, p->c+2) !=  '#'){
                inc = 1;
            }
        case J:
        case L:
            break;
    }
    if(inc){
        clear_piece(p);
        p->c += inc;
        draw_piece(p);
    }
}

void clear_piece(piece_t *p){
    int idx;

    switch (p->type){
        case I:
            if (p->is_horizontal){
                mvprintw(p->r, p->c, "    ");
            }
            else{
                for (idx = 0; idx < 4; idx++){
                    mvprintw(p->r+idx, p->c, " ");
                }
            }
            break;
        case SQUARE:
            for (idx = 0; idx < 2; idx++){
                mvprintw(p->r+idx, p->c, "  ");
            }
            break;
        case J:
            if (p->is_horizontal){
                mvprintw(p->r, p->c, "   ");
                mvprintw(p->r+1, p->c+2, " ");
            }
            else{
                for (idx = 0; idx < 2; idx++){
                    mvprintw(p->r+idx, p->c, " ");
                }
                mvprintw(p->r+2, p->c-1, "  ");
            }
            break;
        case L:
            if (p->is_horizontal){
                mvprintw(p->r, p->c, "   ");
                mvprintw(p->r-1, p->c+2, " ");
            }
            else{
                for (idx = 0; idx < 2; idx++){
                    mvprintw(p->r+idx, p->c, " ");
                }
                mvprintw(p->r+2, p->c, "  ");
            }
            break;
        default:
            break;
    }
}

void draw_piece(piece_t *p){
    int idx;

    switch (p->type){
        case I:
            if (p->is_horizontal){
                mvprintw(p->r, p->c, "####");
            }
            else{
                for (idx = 0; idx < 4; idx++){
                    mvprintw(p->r+idx, p->c, "#");
                }
            }
            break;
        case SQUARE:
            for (idx = 0; idx < 2; idx++){
                mvprintw(p->r+idx, p->c, "##");
            }
            break;
        case J:
            if (p->is_horizontal){
                mvprintw(p->r, p->c, "###");
                mvprintw(p->r+1, p->c+2, "#");
            }
            else {
                for (idx = 0; idx < 2; idx++){
                    mvprintw(p->r+idx, p->c, "#");
                }
                mvprintw(p->r+2, p->c-1, "###");
            }
            break;
        case L:
            if (p->is_horizontal){
                mvprintw(p->r, p->c, "###");
                mvprintw(p->r-1, p->c+2, "#");
            }
            else{
                for (idx = 0; idx < 2; idx++){
                    mvprintw(p->r+idx, p->c, "#");
                }
                mvprintw(p->r+2, p->c, "##");
            }
            break;
        default:
            break;

    }
}

void clear_lines(){
    int r, c;
    bool clear_line = true;
    char blank_line[WIDTH];

    memset(blank_line, ' ', WIDTH-1);
    for (r = HEIGHT-1; r > 0; r--){
        for (c = 1; c < WIDTH; c++){
            if (mvinch(r, c) != '#') {
                clear_line = false;
            }
        }
        if (clear_line){
            score += 1000;
            mvprintw(r, 1, blank_line);
        }
        else{
            clear_line = true;
        }
    }
}
