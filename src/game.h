#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

#define BOARD_W 10
#define BOARD_H 20
#define N_SHAPES 7
#define LINES_PER_LEVEL 10
#define MAX_START_LEVEL 9
#define MAX_LEVEL 15
#define SPRINT_LINES 40
#define ULTRA_MS (2 * 60 * 1000L)

typedef enum { MODE_MARATHON, MODE_SPRINT, MODE_ULTRA, N_MODES } Mode;

typedef struct {
    int type, rot, y, x;
} Piece;

typedef struct {
    int board[BOARD_H][BOARD_W]; /* 0 = empty, otherwise shape type + 1 */
    Piece cur;
    int next;
    int hold; /* shape type, or -1 when the hold slot is empty */
    bool hold_used;
    int bag[N_SHAPES];
    int bag_pos;
    Mode mode;
    int score, lines, start_level, pieces;
    int combo; /* consecutive line-clearing locks, -1 when the chain is broken */
    bool back_to_back;
    long elapsed_ms;
    bool finished; /* no more input is accepted */
    bool won;      /* objective reached: sprint cleared or ultra timer ran out */
} Game;

const char *mode_name(Mode mode);

int shape_size(int type);
/* Is cell (r, c) of the piece's grid filled, after applying its rotation? */
bool piece_cell(const Piece *p, int r, int c);

void game_init(Game *g, Mode mode, int start_level);
bool game_fits(const Game *g, Piece p);
int game_level(const Game *g);
long game_drop_delay_ms(const Game *g);

bool game_move(Game *g, int dy, int dx);
/* dir: +1 clockwise, -1 counter-clockwise. Tries small horizontal wall kicks. */
bool game_rotate(Game *g, int dir);
bool game_soft_drop(Game *g);
void game_hard_drop(Game *g);
bool game_hold(Game *g);
/* One gravity step: move down, or lock the piece if it cannot move. */
void game_tick(Game *g);
void game_lock(Game *g);
void game_advance_time(Game *g, long dt_ms);

/* Row the current piece would land on if hard-dropped. */
int game_ghost_y(const Game *g);

#endif
