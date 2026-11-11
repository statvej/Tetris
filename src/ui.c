#include "ui.h"

#include <ctype.h>
#include <ncurses.h>
#include <stdio.h>
#include <string.h>

#define MIN_ROWS 24
#define MIN_COLS 50
#define PANEL_X 25 /* panel column, relative to the board's left edge */
#define VIEW_W 47  /* board plus side panel */

enum { PAIR_SOLID = 1, PAIR_GHOST = 8, PAIR_MENU = 15 };

static const short SHAPE_COLORS[N_SHAPES] = {COLOR_CYAN, COLOR_YELLOW, COLOR_MAGENTA, COLOR_GREEN,
                                             COLOR_RED,  COLOR_BLUE,   COLOR_WHITE};
static bool colors;

bool ui_init(void)
{
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
        return false;
    }
    colors = has_colors();
    if (colors)
    {
        start_color();
        for (int i = 0; i < N_SHAPES; i++)
        {
            init_pair(PAIR_SOLID + i, COLOR_BLACK, SHAPE_COLORS[i]);
            init_pair(PAIR_GHOST + i, SHAPE_COLORS[i], COLOR_BLACK);
        }
        init_pair(PAIR_MENU, COLOR_MAGENTA, COLOR_CYAN);
    }
    return true;
}

void ui_shutdown(void)
{
    endwin();
}

int ui_get_key(int timeout_ms)
{
    timeout(timeout_ms);
    int key = getch();
    return key == ERR ? -1 : key;
}

void ui_format_time(char *buf, size_t size, long ms)
{
    snprintf(buf, size, "%ld:%02ld.%ld", ms / 60000, ms / 1000 % 60, ms / 100 % 10);
}

static void center(int y, int attr, const char *text)
{
    attron(attr);
    mvaddstr(y, (COLS - (int)strlen(text)) / 2, text);
    attroff(attr);
}

static void board_origin(int *top, int *left)
{
    *top = (LINES - BOARD_H - 1) / 2;
    *left = (COLS - VIEW_W) / 2;
}

static void draw_cell(int y, int x, int type, bool ghost, int theme)
{
    bool color = colors && theme == 0;
    const char *glyph;
    int attr;

    if (ghost)
    {
        glyph = "::";
        attr = color ? COLOR_PAIR(PAIR_GHOST + type) : A_DIM;
    }
    else if (color)
    {
        glyph = "  ";
        attr = COLOR_PAIR(PAIR_SOLID + type);
    }
    else
    {
        glyph = "[]";
        attr = A_BOLD;
    }
    attron(attr);
    mvaddstr(y, x, glyph);
    attroff(attr);
}

static void draw_preview(int y, int x, int type, int theme, bool dim)
{
    Piece p = {type, 0, 0, 0};
    int n = shape_size(type);

    if (dim)
        attron(A_DIM);
    for (int r = 0; r < n; r++)
        for (int c = 0; c < n; c++)
            if (piece_cell(&p, r, c))
                draw_cell(y + r, x + 2 * c, type, false, theme);
    if (dim)
        attroff(A_DIM);
}

static void draw_panel(const Game *g, int top, int px, int theme)
{
    char t[24];

    attron(A_BOLD);
    mvaddstr(top, px, mode_name(g->mode));
    attroff(A_BOLD);
    mvprintw(top + 1, px, "Score: %d", g->score);
    if (g->mode == MODE_SPRINT)
        mvprintw(top + 2, px, "Lines: %d/%d", g->lines, SPRINT_LINES);
    else
        mvprintw(top + 2, px, "Lines: %d", g->lines);
    mvprintw(top + 3, px, "Level: %d", game_level(g));
    ui_format_time(t, sizeof t, g->mode == MODE_ULTRA ? ULTRA_MS - g->elapsed_ms : g->elapsed_ms);
    mvprintw(top + 4, px, "%s: %s", g->mode == MODE_ULTRA ? "Left" : "Time", t);
    mvprintw(top + 5, px, "Pieces: %d", g->pieces);
    if (g->combo > 0)
        mvprintw(top + 6, px, "Combo x%d%s", g->combo, g->back_to_back ? " B2B" : "");
    else if (g->back_to_back)
        mvprintw(top + 6, px, "Back-to-back");

    mvaddstr(top + 7, px, "Hold:");
    if (g->hold >= 0)
        draw_preview(top + 8, px, g->hold, theme, g->hold_used);
    mvaddstr(top + 12, px, "Next:");
    draw_preview(top + 13, px, g->next, theme, false);

    mvaddstr(top + 17, px, "</> move  v soft");
    mvaddstr(top + 18, px, "Up/X rot  Z ccw");
    mvaddstr(top + 19, px, "Space drop C hold");
    mvaddstr(top + 20, px, "P pause  Q quit");
}

void ui_draw_game(const Game *g, const Settings *s)
{
    int top, left;
    board_origin(&top, &left);
    erase();

    for (int r = 0; r < BOARD_H; r++)
    {
        mvaddch(top + r, left, '|');
        mvaddch(top + r, left + 2 * BOARD_W + 1, '|');
        for (int c = 0; c < BOARD_W; c++)
            if (g->board[r][c])
                draw_cell(top + r, left + 1 + 2 * c, g->board[r][c] - 1, false, s->theme);
    }
    mvhline(top + BOARD_H, left, '=', 2 * BOARD_W + 2);

    if (!g->finished)
    {
        int n = shape_size(g->cur.type), ghost_y = game_ghost_y(g);
        for (int r = 0; r < n; r++)
            for (int c = 0; c < n; c++)
            {
                if (!piece_cell(&g->cur, r, c))
                    continue;
                int x = left + 1 + 2 * (g->cur.x + c);
                if (s->ghost && ghost_y != g->cur.y)
                    draw_cell(top + ghost_y + r, x, g->cur.type, true, s->theme);
            }
        for (int r = 0; r < n; r++)
            for (int c = 0; c < n; c++)
                if (piece_cell(&g->cur, r, c))
                    draw_cell(top + g->cur.y + r, left + 1 + 2 * (g->cur.x + c), g->cur.type, false, s->theme);
    }
    draw_panel(g, top, left + PANEL_X, s->theme);
    refresh();
}

void ui_banner(const char *text)
{
    int top, left;
    char padded[32];
    board_origin(&top, &left);
    snprintf(padded, sizeof padded, " %s ", text);
    attron(A_REVERSE | A_BOLD);
    mvaddstr(top + BOARD_H / 2, left + 1 + (2 * BOARD_W - (int)strlen(padded)) / 2, padded);
    attroff(A_REVERSE | A_BOLD);
    refresh();
}

void ui_draw_menu(const char *const labels[], int count, int selected)
{
    int top = (LINES - count - 6) / 2;
    int highlight = colors ? COLOR_PAIR(PAIR_MENU) : A_REVERSE;

    erase();
    center(top, A_BOLD, "T E T R I S");
    for (int i = 0; i < count; i++)
        center(top + 3 + i, i == selected ? highlight : 0, labels[i]);
    center(top + count + 4, A_DIM, "Up/Down select   Left/Right change   Enter confirm");
    refresh();
}

void ui_show_scores(const HighScores *hs)
{
    int y = (LINES - 21) / 2;
    char line[64], t[24];

    erase();
    center(y++, A_BOLD, "HIGH SCORES");
    for (int m = 0; m < N_MODES; m++)
    {
        center(y++, A_UNDERLINE, mode_name((Mode)m));
        for (int i = 0; i < TOP_N; i++)
        {
            if (i >= hs->count[m])
            {
                snprintf(line, sizeof line, "%d. ---", i + 1);
            }
            else
            {
                const ScoreEntry *e = &hs->entry[m][i];
                ui_format_time(t, sizeof t, e->time_ms);
                if (m == MODE_SPRINT)
                    snprintf(line, sizeof line, "%d. %s  %s  (%d lines)", i + 1, e->name, t, e->lines);
                else
                    snprintf(line, sizeof line, "%d. %s  %d  (%d lines)", i + 1, e->name, e->score, e->lines);
            }
            center(y++, A_NORMAL, line);
        }
    }
    center(y + 1, A_DIM, "press any key");
    refresh();
    ui_get_key(-1);
}

void ui_show_text(const char *text)
{
    erase();
    center(LINES / 2, A_NORMAL, text);
    refresh();
    ui_get_key(-1);
}

void ui_prompt_name(char out[NAME_LEN])
{
    int len = 0;
    char line[32];

    out[0] = '\0';
    for (;;)
    {
        erase();
        center(LINES / 2 - 2, A_BOLD, "NEW HIGH SCORE!");
        snprintf(line, sizeof line, "Initials: %-3s", out);
        center(LINES / 2, A_NORMAL, line);
        center(LINES / 2 + 2, A_DIM, "Enter to confirm");
        refresh();

        int ch = ui_get_key(-1);
        if (ch == '\n' || ch == KEY_ENTER)
            return;
        if ((ch == KEY_BACKSPACE || ch == 127 || ch == '\b') && len > 0)
            out[--len] = '\0';
        else if (ch >= 0 && ch < 256 && isalnum(ch) && len < NAME_LEN - 1)
        {
            out[len++] = (char)toupper(ch);
            out[len] = '\0';
        }
    }
}
