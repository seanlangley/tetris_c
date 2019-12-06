#ifndef TETRIS_H
#define TETRIS_H
#define WIDTH 15
#define HEIGHT 30
#define MAX_PIECES 128

typedef struct piece{
    int r;
    int c;
    char shape[4][4];
    bool flipped;
} piece_t;

void draw_well(void);
void init_curses(void);
piece_t *add_piece(void);
void update_curr_piece(void);
void clear_scr(void);

#endif //TETRIS_H
