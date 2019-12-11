#include <ncurses.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "tetris.h"

piece_t *curr_piece = NULL;
piece_t *next_pieces[3];
char error[32], buffer[32];
unsigned int score = 0, level = 1;
bool game_over = false;

int main(){
    int ch, idx;

    srand(time(NULL));
    init_curses();
    draw_well();
    curr_piece = add_piece();
    mvprintw(4, 18, "Next Pieces");
    for (idx = 0; idx < 3; idx++){
        next_pieces[idx] = add_piece();
     }
    draw_next_pieces();
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
            for (idx = 0; idx < 3; idx++){
                clear_piece(next_pieces[idx]);
            }
            curr_piece = next_pieces[0];
            curr_piece->r = 1;
            curr_piece->c = WIDTH/2;
            next_pieces[0] = next_pieces[1];
            next_pieces[1] = next_pieces[2];
            next_pieces[2] = add_piece();
            draw_next_pieces();
        }
        if (game_over){
            break;
        }
        usleep(1000*100);
    }
    endwin();
    return 0;
}

void draw_next_pieces(){
    int idx;
    
    for(idx = 0; idx < 3; idx++){
        next_pieces[idx]->r = 7;
        next_pieces[idx]->c = 18+idx*6;
        draw_piece(next_pieces[idx]);
    }
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
    int idx;

    switch (p->type){
        case I:
            if ((!p->is_horizontal && mvinch(p->r+4, p->c) != ' ') ||
                    (p->is_horizontal && mvinch(p->r+1, p->c) != ' '))  {
                make_new_piece = true;
            }
            break;
        case SQUARE:
            for (idx = 0; idx < 2; idx++){
                if (mvinch(p->r+2, p->c+idx) == '#'){
                    make_new_piece = true;
                }
            }
            break;
        case J:
            if (p->is_horizontal){
                if (mvinch(p->r+1, p->c) == '#' || mvinch(p->r+1, p->c+1) == '#' ||
                    mvinch(p->r+2, p->c+2) == '#'){
                    make_new_piece = true;
                }
            }
            else if (mvinch(p->r+3, p->c-1) == '#' ||
                     mvinch(p->r+3, p->c) == '#'){
                make_new_piece = true;
            }
        case L:
            if (p->is_horizontal){
                for (idx = 0; idx < 3; idx++){
                    if (mvinch(p->r+1, p->c+idx) == '#'){
                        make_new_piece = true;
                    }
                }
            }
            else{
                if (mvinch(p->r+3, p->c) == '#' ||
                    mvinch(p->r+3, p->c+1) == '#'){
                    make_new_piece = true;
                }
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
    int inc = 0, idx;

    switch (p->type){
        case I:
            if (direction == LEFT){
                inc = -1;
            }
            else if (direction == RIGHT) {
                inc = 1;
            }
            if (p->is_horizontal && direction == LEFT){
                if (mvinch(p->r, p->c-1) == '#'){
                    inc = 0;
                }
            }
            else if (p->is_horizontal && direction == RIGHT){
                if (mvinch(p->r, p->c+4) == '#'){
                    inc = 0;
                }
            }
            else{ //Vertical
                for (idx = 0; idx < 4; idx++){
                    if (mvinch(p->r+idx, p->c+inc) == '#'){
                        inc = 0;
                    }
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
            if (direction == LEFT){
            }
            else{
            }
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
