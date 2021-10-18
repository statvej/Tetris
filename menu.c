
// #include <ncurses.h>
// #include <stdlib.h>
// #include <string.h>

// int		main(void)
// {
// 	int		win_x; //Max Win X
// 	int		win_y; //Max Win Y
// 	int		option = 1; //Current Selected Option
// 	int		c_x; //Center X of screen
// 	int		c_y; //Center Y of screen
// 	int		titleboxlen; //Strlen of titlebox
// 	titleboxlen = strlen("=========================================================================="); //Get Title Box Length To Center It

// 	char *o_1p = "1 Player"; //Just The Option Names, You Can Change Em
// 	char *o_2p = "2 Players";
// 	char *o_help = "Help";
// 	char *o_credits = "Credits";
// 	char *o_quit = "Quit";

// 	initscr();
// 	cbreak();
// 	noecho();
// 	curs_set(FALSE);
// 	start_color();
// 	init_pair(1, COLOR_BLACK, COLOR_WHITE);

// 	getmaxyx(stdscr, win_y, win_x);
// 	WINDOW *win_menu = newwin(win_y, win_x, 0, 0);

// 	keypad(win_menu, true);

// 	while (1)
// 	{
// 		wclear(win_menu);
// 		getmaxyx(win_menu, win_y, win_x);
// 		c_x = win_x / 2; //Find Middle of X
// 		c_y = win_y / 2; //Find Middle of Y

// 		//Render Fancy Boxes ETC
// 		//The Text centering takes half the screen, then further subtracts half the strlen to center it perfectly. Just for you mister OCD
// 		mvwprintw(win_menu, c_y - 10, c_x - (titleboxlen / 2), "==========================================================================");
// 		mvwprintw(win_menu, c_y - 9, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y - 8, c_x - (titleboxlen / 2), "||                                TETRIS                                ||");
// 		mvwprintw(win_menu, c_y - 7, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y - 6, c_x - (titleboxlen / 2), "==========================================================================");
// 		mvwprintw(win_menu, c_y - 5, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y - 4, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y - 3, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y - 2, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y - 1, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y + 1, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y + 2, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y + 3, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y + 4, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y + 5, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y + 6, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y + 7, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y + 8, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y + 9, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y + 10, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y + 11, c_x - (titleboxlen / 2), "||                                                                      ||");
// 		mvwprintw(win_menu, c_y + 12, c_x - (titleboxlen / 2), "==========================================================================");

// 		//Render Out Buttons, Inverse Video Of Selected
// 		if (option == 1)
// 		{
// 			wattron(win_menu, COLOR_PAIR(1));
// 			mvwprintw(win_menu, c_y - 1, c_x - (strlen(o_1p) / 2), o_1p);
// 			wattroff(win_menu, COLOR_PAIR(1));
// 		}
// 		else
// 			mvwprintw(win_menu, c_y - 1, c_x - (strlen(o_1p) / 2), o_1p);

// 		if (option == 2)
// 		{
// 			wattron(win_menu, COLOR_PAIR(1));
// 			mvwprintw(win_menu, c_y + 1, c_x - (strlen(o_2p) / 2), o_2p);
// 			wattroff(win_menu, COLOR_PAIR(1));
// 		}
// 		else
// 			mvwprintw(win_menu, c_y + 1, c_x - (strlen(o_2p) / 2), o_2p);

// 		if (option == 3)
// 		{
// 			wattron(win_menu, COLOR_PAIR(1));
// 			mvwprintw(win_menu, c_y + 3, c_x - (strlen(o_help) / 2), o_help);
// 			wattroff(win_menu, COLOR_PAIR(1));
// 		}
// 		else
// 			mvwprintw(win_menu, c_y + 3, c_x - (strlen(o_help) / 2), o_help);

// 		if (option == 4)
// 		{
// 			wattron(win_menu, COLOR_PAIR(1));
// 			mvwprintw(win_menu, c_y + 5, c_x - (strlen(o_credits) / 2), o_credits);
// 			wattroff(win_menu, COLOR_PAIR(1));
// 		}
// 		else
// 			mvwprintw(win_menu, c_y + 5, c_x - (strlen(o_credits) / 2), o_credits);

// 		if (option == 5)
// 		{
// 			wattron(win_menu, COLOR_PAIR(1));
// 			mvwprintw(win_menu, c_y + 7, c_x - (strlen(o_quit) / 2), o_quit);
// 			wattroff(win_menu, COLOR_PAIR(1));
// 		}
// 		else
// 			mvwprintw(win_menu, c_y + 7, c_x - (strlen(o_quit) / 2), o_quit);

// 		//Update Screen
// 		wrefresh(win_menu);

// 		//Key Hooks
// 		switch(wgetch(win_menu))
// 		{
// 			case KEY_UP:
// 				if (option == 1)
// 					option = 5;
// 				else
// 					option -= 1;
// 				break;
// 			case KEY_DOWN:
// 				if (option == 5)
// 					option = 1;
// 				else
// 					option += 1;
// 				break;
// 			case 10: //10 Being Enter Key
// 				if (option == 1)
// 				{
// 					//Do One Player Stuff
// 				}

// 				if (option == 2)
// 				{
// 					//Do Two Player Stuff
// 				}

// 				if (option == 3)
// 				{
// 					//Help Stuff
// 				}

// 				if (option == 4)
// 				{
// 					//Credits Stuff
// 				}

// 				if (option == 5)
// 				{
// 					//Quit The Game
// 					endwin();
// 					curs_set(TRUE);
// 					system("clear");
// 					return (0);
// 				}
// 				break;
// 		}
// 	}
// }

#include <ncurses.h>
#include <string.h>
#include <stdio.h>

int main()
{
  char add;

  int y = 20;
  int x = 45;
  initscr();
  raw();
  cbreak();
  noecho();
  curs_set(FALSE);

  if (has_colors() == FALSE)
  {
    endwin();
    printf("Your terminal does not support color\n");
  }
  start_color();
  init_pair(1, COLOR_BLACK, COLOR_WHITE);

  while (y != 30)
  {
    mvprintw(20, 45, "==========================");
    mvprintw(30, 45, "==========================");
    mvaddch(y, x, '|');
    mvaddch(y, x + 25, '|');
    refresh();
    ++y;
  }
  char *message_controls = "Controls";
  char *message_quit = "Quit";
  char *message_start = "Start the game";

  mvprintw(22, 50, message_start);
  mvprintw(24, 50, message_controls);
  mvprintw(26, 50, message_quit);

  while ((add = getch()) != 's' || (add = getch()) != 'c' || (add = getch()) != 'q')
  {
    switch (add)
    {
    case 's':

      mvprintw(22, 50, message_start);

      break;

    case 'c':

      mvprintw(24, 50, message_controls);

      break;

    case 'q':

      mvprintw(26, 50, message_quit);

      break;
    }
    refresh();
  }

  endwin();

  return 0;
}
