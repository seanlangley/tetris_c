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

    new_piece->type = (enum piece_type)(rand() % PIECE_TYPES);
    switch (new_piece->type){
        case I:
            strncpy(new_piece->shape[0], " #  ", 5);
            strncpy(new_piece->shape[1], " #  ", 5);
            strncpy(new_piece->shape[2], " #  ", 5);
            strncpy(new_piece->shape[3], " #  ", 5);
            if (mvinch(4, WIDTH/2) == '#'){
                game_over = true;
            }
            break;
        case SQUARE:
            strncpy(new_piece->shape[0], "    ", 5);
            strncpy(new_piece->shape[1], " ## ", 5);
            strncpy(new_piece->shape[2], " ## ", 5);
            strncpy(new_piece->shape[3], "    ", 5);
            if (mvinch(3, WIDTH/2) == '#' || mvinch(3, WIDTH/2+1) == '#'){
                game_over = true;
            }
            break;
        case J:
            strncpy(new_piece->shape[0], "  # ", 5);
            strncpy(new_piece->shape[1], "  # ", 5);
            strncpy(new_piece->shape[2], " ## ", 5);
            strncpy(new_piece->shape[3], "    ", 5);
            if (mvinch(3, WIDTH/2-1) == '#' || mvinch(3, WIDTH/2) == '#'){
                game_over = true;
            }
            break;
        case L:
            strncpy(new_piece->shape[0], " #  ", 5);
            strncpy(new_piece->shape[1], " #  ", 5);
            strncpy(new_piece->shape[2], " ## ", 5);
            strncpy(new_piece->shape[3], "    ", 5);
            if (mvinch(3, WIDTH/2) == '#' || mvinch(3, WIDTH/2+1) == '#'){
                game_over = true;
            }
            break;
        case S:
            strncpy(new_piece->shape[0], "    ", 5);
            strncpy(new_piece->shape[1], "  ##", 5);
            strncpy(new_piece->shape[2], " ## ", 5);
            strncpy(new_piece->shape[3], "    ", 5);
            break;
        case Z:
            strncpy(new_piece->shape[0], "    ", 5);
            strncpy(new_piece->shape[1], "##  ", 5);
            strncpy(new_piece->shape[2], " ## ", 5);
            strncpy(new_piece->shape[3], "    ", 5);
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
    int r, c;

    clear_piece(p);
    for (r = 0; r < 4; r++){
        for (c = 0; c < 4; c++){
            if (p->shape[r][c] == '#' && mvinch(p->r+r+1, p->c+c) == '#'){
                make_new_piece = true;
            }
        }
    }
    if (make_new_piece){
        draw_piece(p);
        free(curr_piece);
        curr_piece = NULL;
        clear_lines();
        return;
    }
    p->r++;
    if (p->do_flip){
        do_flip(p);
        p->do_flip = false;
    }
    draw_piece(p);
}

void do_flip(piece_t *p){
    int r, c;
    char new_shape[4][5];
    
    for (r = 0; r < 4; r++){
        for (c = 0; c < 4; c++){
            new_shape[c][r] = p->shape[r][c];
        }
        new_shape[r][4] = '\0';
    }
    for (r = 0; r < 4; r++){
        strncpy(p->shape[r], new_shape[r], 5);
    }
}

void move_horizontally(direction_t direction){
    piece_t *p = curr_piece;
    int inc = 0, r, c;

    direction == LEFT ? (inc = -1) : (inc = 1);
    clear_piece(p);
    for (r = 0; r < 4; r++){
        for (c = 0; c < 4; c++){
            if (p->shape[r][c] == '#' && mvinch(p->r+r, p->c+c+inc) == '#'){
                inc = 0;
            }
        }
    }

    p->c += inc;
    draw_piece(p);
}

void clear_piece(piece_t *p){
    int r, c;

    for (r = 0; r < 4; r++){
        for (c = 0; c < 4; c++){
            if (p->shape[r][c] == '#'){
                mvprintw(p->r+r, p->c+c, " ");
            }
        }
    }
}

void draw_piece(piece_t *p){
    int r, c;

    for (r = 0; r < 4; r++){
        for (c = 0; c < 4; c++){
            if (p->shape[r][c] == '#'){
                mvprintw(p->r+r, p->c+c, "#");
            }
        }
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
