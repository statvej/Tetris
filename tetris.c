#define _POSIX_C_SOURCE 200809L
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BOARD_W 10
#define BOARD_H 20
#define MAX_LEVEL 9
#define LINES_PER_LEVEL 10
#define VIEW_W 41 /* board + side panel, in terminal columns */
#define MIN_ROWS 24
#define MIN_COLS 44

/* Each shape is an n x n grid; 'X' marks a filled cell. Rotation is computed. */
static const struct { int n; const char *cells; } SHAPES[] = {
    {4, "....XXXX........"}, /* I */
    {2, "XXXX"},             /* O */
    {3, ".X.XXX..."},        /* T */
    {3, ".XXXX...."},        /* S */
    {3, "XX..XX..."},        /* Z */
    {3, "X..XXX..."},        /* J */
    {3, "..XXXX..."},        /* L */
};
static const short SHAPE_COLORS[] = {COLOR_CYAN, COLOR_YELLOW, COLOR_MAGENTA, COLOR_GREEN,
                                     COLOR_RED,  COLOR_BLUE,   COLOR_WHITE};
#define N_SHAPES ((int)(sizeof SHAPES / sizeof *SHAPES))

typedef struct { int type, rot, y, x; } Piece;

typedef struct {
    int board[BOARD_H][BOARD_W]; /* 0 = empty, otherwise shape type + 1 */
    Piece cur;
    int next, score, lines, start_level;
} Game;

static int level_of(const Game *g) { return g->start_level + g->lines / LINES_PER_LEVEL; }

static long drop_delay_ms(int level)
{
    long ms = 1000 - 100L * level;
    return ms < 100 ? 100 : ms;
}

static long now_ms(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000L + t.tv_nsec / 1000000;
}

/* Is cell (r, c) of the piece's n x n grid filled, after rotating clockwise? */
static bool filled(const Piece *p, int r, int c)
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

static bool fits(const Game *g, Piece p)
{
    int n = SHAPES[p.type].n;
    for (int r = 0; r < n; r++)
        for (int c = 0; c < n; c++)
        {
            if (!filled(&p, r, c))
                continue;
            int y = p.y + r, x = p.x + c;
            if (y < 0 || y >= BOARD_H || x < 0 || x >= BOARD_W || g->board[y][x])
                return false;
        }
    return true;
}

/* Move and/or rotate the current piece; rotation tries small horizontal kicks. */
static bool try_move(Game *g, int dy, int dx, int drot)
{
    static const int kicks[] = {0, -1, 1, -2, 2};
    int tries = drot ? 5 : 1;
    for (int i = 0; i < tries; i++)
    {
        Piece p = g->cur;
        p.y += dy;
        p.x += dx + kicks[i];
        p.rot = (p.rot + drot) & 3;
        if (fits(g, p))
        {
            g->cur = p;
            return true;
        }
    }
    return false;
}

/* Bring in the next piece; false means it has no room (game over). */
static bool spawn(Game *g)
{
    g->cur = (Piece){g->next, 0, 0, (BOARD_W - SHAPES[g->next].n) / 2};
    g->next = rand() % N_SHAPES;
    return fits(g, g->cur);
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

/* Freeze the current piece, score cleared lines, spawn the next. False = game over. */
static bool settle(Game *g)
{
    static const int line_points[] = {0, 100, 300, 500, 800};
    int n = SHAPES[g->cur.type].n;
    for (int r = 0; r < n; r++)
        for (int c = 0; c < n; c++)
            if (filled(&g->cur, r, c))
                g->board[g->cur.y + r][g->cur.x + c] = g->cur.type + 1;
    int cleared = clear_lines(g);
    g->score += line_points[cleared] * level_of(g);
    g->lines += cleared;
    return spawn(g);
}

static void draw_cell(int y, int x, int type)
{
    int attr = has_colors() ? COLOR_PAIR(type + 1) : A_REVERSE;
    attron(attr);
    mvaddstr(y, x, "  ");
    attroff(attr);
}

static void draw_game(const Game *g, int top, int left)
{
    erase();
    for (int r = 0; r < BOARD_H; r++)
    {
        mvaddch(top + r, left, '|');
        mvaddch(top + r, left + 2 * BOARD_W + 1, '|');
        for (int c = 0; c < BOARD_W; c++)
            if (g->board[r][c])
                draw_cell(top + r, left + 1 + 2 * c, g->board[r][c] - 1);
    }
    mvhline(top + BOARD_H, left, '=', 2 * BOARD_W + 2);

    int n = SHAPES[g->cur.type].n;
    for (int r = 0; r < n; r++)
        for (int c = 0; c < n; c++)
            if (filled(&g->cur, r, c))
                draw_cell(top + g->cur.y + r, left + 1 + 2 * (g->cur.x + c), g->cur.type);

    int px = left + 2 * BOARD_W + 4;
    mvprintw(top, px, "Score: %d", g->score);
    mvprintw(top + 1, px, "Lines: %d", g->lines);
    mvprintw(top + 2, px, "Level: %d", level_of(g));
    mvprintw(top + 4, px, "Next:");
    Piece preview = {g->next, 0, 0, 0};
    n = SHAPES[g->next].n;
    for (int r = 0; r < n; r++)
        for (int c = 0; c < n; c++)
            if (filled(&preview, r, c))
                draw_cell(top + 6 + r, px + 2 * c, g->next);
    mvprintw(top + 12, px, "Move  : <- ->");
    mvprintw(top + 13, px, "Rotate: up");
    mvprintw(top + 14, px, "Soft  : down");
    mvprintw(top + 15, px, "Hard  : space");
    mvprintw(top + 16, px, "P pause  Q quit");
}

/* Print a message over the board and wait for a key. */
static void banner(int top, int left, const char *text)
{
    mvprintw(top + BOARD_H / 2, left + 1 + (2 * BOARD_W - (int)strlen(text)) / 2, "%s", text);
    refresh();
    flushinp();
    timeout(-1);
    getch();
}

static void play(int start_level)
{
    Game g = {.start_level = start_level, .next = rand() % N_SHAPES};
    int top = (LINES - BOARD_H - 1) / 2, left = (COLS - VIEW_W) / 2;
    spawn(&g);

    long last = now_ms();
    for (bool alive = true; alive;)
    {
        draw_game(&g, top, left);
        long wait = last + drop_delay_ms(level_of(&g)) - now_ms();
        timeout(wait > 0 ? wait : 0);

        switch (getch())
        {
        case KEY_LEFT:
        case 'a':
            try_move(&g, 0, -1, 0);
            break;
        case KEY_RIGHT:
        case 'd':
            try_move(&g, 0, 1, 0);
            break;
        case KEY_UP:
        case 'w':
            try_move(&g, 0, 0, 1);
            break;
        case KEY_DOWN:
        case 's':
            g.score += try_move(&g, 1, 0, 0);
            break;
        case ' ':
            while (try_move(&g, 1, 0, 0))
                g.score += 2;
            alive = settle(&g);
            last = now_ms();
            break;
        case 'p':
            draw_game(&g, top, left);
            banner(top, left, "PAUSED");
            last = now_ms();
            break;
        case 'q':
        case 27:
            return;
        }

        if (alive && now_ms() - last >= drop_delay_ms(level_of(&g)))
        {
            last = now_ms();
            if (!try_move(&g, 1, 0, 0))
                alive = settle(&g);
        }
    }

    draw_game(&g, top, left);
    banner(top, left, "GAME OVER");
}

enum { START, LEVEL, CREDITS, QUIT, N_ITEMS };

static int step_level(int level, int d) { return (level - 1 + d + MAX_LEVEL) % MAX_LEVEL + 1; }

static void draw_menu(int sel, int level)
{
    char label[N_ITEMS][16] = {"Start", "", "Credits", "Quit"};
    snprintf(label[LEVEL], sizeof label[LEVEL], "< Level %d >", level);

    erase();
    int top = (LINES - N_ITEMS - 4) / 2;
    mvprintw(top, (COLS - 6) / 2, "TETRIS");
    for (int i = 0; i < N_ITEMS; i++)
    {
        int attr = i == sel ? (has_colors() ? COLOR_PAIR(1) : A_REVERSE) : A_NORMAL;
        attron(attr);
        mvprintw(top + 3 + i, (COLS - (int)strlen(label[i])) / 2, "%s", label[i]);
        attroff(attr);
    }
    refresh();
}

int main(void)
{
    srand(time(NULL));
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    set_escdelay(25);

    if (LINES < MIN_ROWS || COLS < MIN_COLS)
    {
        endwin();
        fprintf(stderr, "Terminal too small: need at least %dx%d\n", MIN_COLS, MIN_ROWS);
        return 1;
    }
    if (has_colors())
    {
        start_color();
        for (int i = 0; i < N_SHAPES; i++)
            init_pair(i + 1, COLOR_BLACK, SHAPE_COLORS[i]);
    }

    int sel = START, level = 1;
    for (;;)
    {
        timeout(-1);
        draw_menu(sel, level);
        switch (getch())
        {
        case KEY_UP:
        case 'w':
            sel = (sel + N_ITEMS - 1) % N_ITEMS;
            break;
        case KEY_DOWN:
        case 's':
            sel = (sel + 1) % N_ITEMS;
            break;
        case KEY_LEFT:
            if (sel == LEVEL)
                level = step_level(level, -1);
            break;
        case KEY_RIGHT:
            if (sel == LEVEL)
                level = step_level(level, 1);
            break;
        case 10:
        case KEY_ENTER:
            if (sel == START)
                play(level);
            else if (sel == LEVEL)
                level = step_level(level, 1);
            else if (sel == CREDITS)
            {
                erase();
                mvprintw(LINES / 2, (COLS - 31) / 2, "yarutiunov, fstaryk, hpinchuktr");
                refresh();
                getch();
            }
            else
                goto done;
            break;
        case 'q':
        case 27:
            goto done;
        }
    }
done:
    endwin();
    return 0;
}
