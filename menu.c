#include <ncurses.h>
#include <stdbool.h>
#define height 20
#define start_y 17
#define width 30
#define start_x 45
char up = '=';
char side = '|';
int x = 0;
int i;
int smth = 0;
int counter = 0;
int ishod_start = 0;
int ishod_level = 1;
int ishod_credits = 2;
int ishod_quit = 3;
int main()
{

    char *options[4] = {"Start", "Level", "Credits", "Quit"};

    initscr();
    cbreak();
    noecho();
    if (has_colors() != true)
    {

        printw("Your terminal does not support colors");
    }

    start_color();
    init_pair(1, COLOR_MAGENTA, COLOR_CYAN);
    init_pair(2, COLOR_BLACK, COLOR_BLACK);

    WINDOW *win = newwin(height+10, width + 2, start_y, start_x);
    refresh();

    wborder(win, (int)side, (int)side, (int)up, (int)up, (int)up, (int)up, (int)up, (int)up);
    nodelay(win, TRUE);
    keypad(win, true);

    wrefresh(win);

    int choise = wgetch(win);

    int c = wgetch(win);
    int a = wgetch(win);
    while (true)
    {
        mvwprintw(win, i + 5, x + 11, options[i]);
        while (1)
        {
            for (i = 0; i < 4; i++)
            {
                if (i == smth)
                {

                    wattron(win, COLOR_PAIR(1));
                    mvwprintw(win, i + 5, x + 11, options[i]);
                    wattroff(win, COLOR_PAIR(1));
                }

                choise = wgetch(win);

                switch (choise)
                {

                case KEY_UP:
                    if (counter > 0)
                    {

                        counter = counter - 1;
                    }
                    smth--;
                    break;

                case KEY_DOWN:
                    if (counter < 3)
                    {

                        counter = counter + 1;
                    }

                    smth++;
                    break;

                case 10:

                    x = smth;
                    if (ishod_start == smth)
                    {

                        while (1)
                        {
                           clear();
                           refresh();
                            mvwprintw(win, 1, 1, "the game has started");
                            refresh();
                            break;

                            // ВЕСЬ ЭТОТ ПРИКОЛ ДОЛЖЕН ЗАПУСКАТЬ ИГРУ
                        }
                        break;
                    }
                    if (ishod_quit == smth)
                    {

                        
                        
                            exit(0);

                            // ВЕСЬ ЭТОТ ПРИКОЛ ДОЛЖЕН ЗАПУСКАТЬ ИГРУ
                        
                    }
                    if (ishod_credits == smth)
                    {
                        clear();
                           refresh();
                            mvwprintw(win, 1, 1, "yarutiunov, fstaryk, hpinchuktr");
                            refresh();
                            break;
                            

                            // ВЕСЬ ЭТОТ ПРИКОЛ ДОЛЖЕН ЗАПУСКАТЬ ИГРУ
                    }
                    
                
                }
            }

            if (choise == 27)
            {

                endwin();
                return 0;
            }
              
        }
      endwin();
    return 0;
    }
}

