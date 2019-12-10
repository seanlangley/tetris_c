#ifndef TETRIS_H
#define TETRIS_H
#define WIDTH 15
#define HEIGHT 15
#define MAX_PIECES 128

typedef enum direction{
    LEFT,
    RIGHT
} direction_t;

enum piece_type{
    I,
    L,
    J,
    SQUARE,
};

typedef struct piece{
    int r;
    int c;
    bool do_flip;
    bool is_horizontal;
    enum piece_type type;
} piece_t;

void draw_well(void);
void init_curses(void);
piece_t *add_piece(void);
void update_curr_piece(void);
void move_horizontally(direction_t);
void clear_piece(piece_t*);
void draw_piece(piece_t*);
void clear_lines(void);
void draw_next_pieces(void);

#endif //TETRIS_H
