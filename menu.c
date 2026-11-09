#include <ncurses.h>
#include <stdio.h>
#define height 20
#define start_y 17
#define width 30
#define start_x 45

enum { START, LEVEL, CREDITS, QUIT, N_OPTIONS };

static void show_message(WINDOW *win, const char *msg)
{
    werase(win);
    box(win, 0, 0);
    mvwprintw(win, 1, 1, "%s", msg);
    wrefresh(win);
    wgetch(win);
}

int main()
{
    char *options[N_OPTIONS] = {"Start", "Level", "Credits", "Quit"};
    int sel = START;

    initscr();
    cbreak();
    noecho();
    curs_set(0);

    if (LINES < start_y + height + 10 || COLS < start_x + width + 2)
    {
        endwin();
        fputs("Terminal too small\n", stderr);
        return 1;
    }

    int highlight = A_REVERSE;
    if (has_colors())
    {
        start_color();
        init_pair(1, COLOR_MAGENTA, COLOR_CYAN);
        highlight = COLOR_PAIR(1);
    }

    WINDOW *win = newwin(height + 10, width + 2, start_y, start_x);
    keypad(win, TRUE);

    while (1)
    {
        werase(win);
        wborder(win, '|', '|', '=', '=', '=', '=', '=', '=');
        for (int i = 0; i < N_OPTIONS; i++)
        {
            if (i == sel)
                wattron(win, highlight);
            mvwprintw(win, i + 5, 11, "%s", options[i]);
            wattroff(win, highlight);
        }
        wrefresh(win);

        switch (wgetch(win))
        {
        case KEY_UP:
            if (sel > 0)
                sel--;
            break;
        case KEY_DOWN:
            if (sel < N_OPTIONS - 1)
                sel++;
            break;
        case 10:
        case KEY_ENTER:
            if (sel == START)
                show_message(win, "the game has started"); // TODO: launch the game here
            else if (sel == CREDITS)
                show_message(win, "yarutiunov, fstaryk, hpinchuktr");
            else if (sel == QUIT)
            {
                endwin();
                return 0;
            }
            break;
        case 27:
            endwin();
            return 0;
        }
    }
}
