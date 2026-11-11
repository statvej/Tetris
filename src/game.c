#include "game.h"

#include <stdlib.h>
#include <string.h>

/* Each shape is an n x n grid; 'X' marks a filled cell. Rotation is computed. */
static const struct {
    int n;
    const char *cells;
} SHAPES[N_SHAPES] = {
    {4, "....XXXX........"}, /* I */
    {2, "XXXX"},             /* O */
    {3, ".X.XXX..."},        /* T */
    {3, ".XXXX...."},        /* S */
    {3, "XX..XX..."},        /* Z */
    {3, "X..XXX..."},        /* J */
    {3, "..XXXX..."},        /* L */
};

static const char *const MODE_NAMES[N_MODES] = {"Marathon", "Sprint", "Ultra"};

const char *mode_name(Mode mode)
{
    return mode >= 0 && mode < N_MODES ? MODE_NAMES[mode] : "?";
}

int shape_size(int type)
{
    return SHAPES[type].n;
}

bool piece_cell(const Piece *p, int r, int c)
{
    int n = SHAPES[p->type].n;
    for (int i = 0; i < p->rot; i++)
    {
        int t = r;
        r = n - 1 - c;
        c = t;
    }
    return SHAPES[p->type].cells[r * n + c] == 'X';
}

bool game_fits(const Game *g, Piece p)
{
    int n = SHAPES[p.type].n;
    for (int r = 0; r < n; r++)
        for (int c = 0; c < n; c++)
        {
            if (!piece_cell(&p, r, c))
                continue;
            int y = p.y + r, x = p.x + c;
            if (y < 0 || y >= BOARD_H || x < 0 || x >= BOARD_W || g->board[y][x])
                return false;
        }
    return true;
}

int game_level(const Game *g)
{
    int level = g->start_level + g->lines / LINES_PER_LEVEL;
    return level > MAX_LEVEL ? MAX_LEVEL : level;
}

long game_drop_delay_ms(const Game *g)
{
    long ms = 1000 - 60L * game_level(g);
    return ms < 60 ? 60 : ms;
}

/* 7-bag randomizer: every shape appears once per bag, so droughts stay short. */
static int next_from_bag(Game *g)
{
    if (g->bag_pos >= N_SHAPES)
    {
        for (int i = 0; i < N_SHAPES; i++)
            g->bag[i] = i;
        for (int i = N_SHAPES - 1; i > 0; i--)
        {
            int j = rand() % (i + 1), t = g->bag[i];
            g->bag[i] = g->bag[j];
            g->bag[j] = t;
        }
        g->bag_pos = 0;
    }
    return g->bag[g->bag_pos++];
}

/* Put a piece at the top of the board; false means it has no room. */
static bool place(Game *g, int type)
{
    g->cur = (Piece){type, 0, 0, (BOARD_W - SHAPES[type].n) / 2};
    if (game_fits(g, g->cur))
        return true;
    g->finished = true;
    g->won = false;
    return false;
}

static void spawn(Game *g)
{
    int type = g->next;
    g->next = next_from_bag(g);
    g->hold_used = false;
    g->pieces++;
    place(g, type);
}

void game_init(Game *g, Mode mode, int start_level)
{
    memset(g, 0, sizeof *g);
    g->mode = mode;
    g->start_level = start_level < 1 ? 1 : start_level > MAX_START_LEVEL ? MAX_START_LEVEL : start_level;
    g->hold = -1;
    g->combo = -1;
    g->bag_pos = N_SHAPES;
    g->next = next_from_bag(g);
    spawn(g);
}

bool game_move(Game *g, int dy, int dx)
{
    if (g->finished)
        return false;
    Piece p = g->cur;
    p.y += dy;
    p.x += dx;
    if (!game_fits(g, p))
        return false;
    g->cur = p;
    return true;
}

bool game_rotate(Game *g, int dir)
{
    static const int kicks[] = {0, -1, 1, -2, 2};
    if (g->finished)
        return false;
    for (size_t i = 0; i < sizeof kicks / sizeof *kicks; i++)
    {
        Piece p = g->cur;
        p.x += kicks[i];
        p.rot = (p.rot + dir) & 3;
        if (game_fits(g, p))
        {
            g->cur = p;
            return true;
        }
    }
    return false;
}

bool game_soft_drop(Game *g)
{
    if (!game_move(g, 1, 0))
        return false;
    g->score++;
    return true;
}

void game_hard_drop(Game *g)
{
    if (g->finished)
        return;
    while (game_move(g, 1, 0))
        g->score += 2;
    game_lock(g);
}

bool game_hold(Game *g)
{
    if (g->finished || g->hold_used)
        return false;
    int held = g->hold;
    g->hold = g->cur.type;
    if (held < 0)
        spawn(g);
    else
        place(g, held);
    g->hold_used = true;
    return true;
}

void game_tick(Game *g)
{
    if (!g->finished && !game_move(g, 1, 0))
        game_lock(g);
}

static int clear_lines(Game *g)
{
    int cleared = 0;
    for (int r = BOARD_H - 1; r >= 0; r--)
    {
        bool full = true;
        for (int c = 0; c < BOARD_W; c++)
            full = full && g->board[r][c];
        if (!full)
            continue;
        memmove(g->board[1], g->board[0], r * sizeof g->board[0]);
        memset(g->board[0], 0, sizeof g->board[0]);
        cleared++;
        r++; /* the rows shifted down, so check this row again */
    }
    return cleared;
}

static bool board_empty(const Game *g)
{
    for (int r = 0; r < BOARD_H; r++)
        for (int c = 0; c < BOARD_W; c++)
            if (g->board[r][c])
                return false;
    return true;
}

static void score_clear(Game *g, int cleared)
{
    static const int line_points[] = {0, 100, 300, 500, 800};
    int level = game_level(g);

    if (!cleared)
    {
        g->combo = -1;
        return;
    }
    int gain = line_points[cleared] * level;
    if (cleared == 4)
    {
        if (g->back_to_back)
            gain += gain / 2;
        g->back_to_back = true;
    }
    else
        g->back_to_back = false;
    g->combo++;
    gain += 50 * g->combo * level;
    if (board_empty(g))
        gain += 1500 * level;
    g->score += gain;
    g->lines += cleared;
}

void game_lock(Game *g)
{
    if (g->finished)
        return;
    int n = SHAPES[g->cur.type].n;
    for (int r = 0; r < n; r++)
        for (int c = 0; c < n; c++)
            if (piece_cell(&g->cur, r, c))
                g->board[g->cur.y + r][g->cur.x + c] = g->cur.type + 1;
    score_clear(g, clear_lines(g));

    if (g->mode == MODE_SPRINT && g->lines >= SPRINT_LINES)
    {
        g->finished = g->won = true;
        return;
    }
    spawn(g);
}

void game_advance_time(Game *g, long dt_ms)
{
    if (g->finished)
        return;
    g->elapsed_ms += dt_ms;
    if (g->mode == MODE_ULTRA && g->elapsed_ms >= ULTRA_MS)
    {
        g->elapsed_ms = ULTRA_MS;
        g->finished = g->won = true;
    }
}

int game_ghost_y(const Game *g)
{
    Piece p = g->cur;
    for (;;)
    {
        p.y++;
        if (!game_fits(g, p))
            return p.y - 1;
    }
}
