#include <ncurses.h>
#include <unistd.h>

#define WIDTH 40
#define HEIGHT 20
#define DROP_SPEED 1000000
int position = 0;
int position_up = 0;
struct s_coord
{
    int row;
    int column;
};

typedef struct s_figure
{
    int color_pair;

    int width1;
    int height1;

    int width2;
    int height2;

    int width3;
    int height3;

    int width4;
    int height4;

    int thicc;
} t_figure;

void playground()
{
    int count_h = 0;

    int count_f = 1;
    while (count_h < HEIGHT)
    {
        int count_w = 1;
        mvaddch(count_h, 0, '|');
        while (count_w < WIDTH - 1)
        {

            mvaddch(count_h, count_w, ':');
            ++count_w;
        }
        mvaddch(count_h, count_w, '|');
        ++count_h;
    }
    mvaddch(HEIGHT, 0, '\\');
    while (count_f < WIDTH - 1)
    {

        mvaddch(HEIGHT, count_f, '_');
        ++count_f;
    }
    mvaddch(HEIGHT, WIDTH - 1, '/');
}

t_figure *figure_printer(t_figure block, int input)
{
    t_figure *current;
    switch (input) //input calculation with taking into account playground limits
    {
    case 'a':
        if (position > (WIDTH / 2 - 4) * (-1))
        {
            position = position - 2;
        }
        break;
    case 'd':
        if (position < (WIDTH / 2 - block.thicc + 1))
        {
            position = position + 2;
        }
    default:
        break;
    }

    block.width1 = block.width1 + position - 1;
    block.width2 = block.width2 + position - 1;
    block.width3 = block.width3 + position - 1;
    block.width4 = block.width4 + position - 1;

    //color pairs
    init_pair(1, COLOR_YELLOW, COLOR_YELLOW); //For O-Block
    init_pair(2, COLOR_CYAN, COLOR_CYAN);     // for I-BLOCK

    attron(COLOR_PAIR(block.color_pair)); //printing fase
    mvprintw(block.height1, block.width1, "  ");
    mvprintw(block.height2, block.width2, "  ");
    mvprintw(block.height3, block.width3, "  ");
    mvprintw(block.height4, block.width4, "  ");
    attroff(COLOR_PAIR(block.color_pair));
    return current;
}
int main()
{
    int ch;
    t_figure *current_blocks;
    t_figure o_block; //O-BLOCK parameters
    o_block.color_pair = 1;
    o_block.height1 = 1;
    o_block.height2 = 1;
    o_block.height3 = 2;
    o_block.height4 = 2;
    o_block.width1 = WIDTH / 2 - 2;
    o_block.width2 = WIDTH / 2;
    o_block.width3 = WIDTH / 2 - 2;
    o_block.width4 = WIDTH / 2;
    o_block.thicc = 4;
    t_figure i_block; //I-BLOCK parameters
    i_block.color_pair = 2;
    i_block.height1 = 1;
    i_block.height2 = 1;
    i_block.height3 = 1;
    i_block.height4 = 1;
    i_block.width1 = WIDTH / 2 - 2;
    i_block.width2 = WIDTH / 2;
    i_block.width3 = WIDTH / 2 + 2;
    i_block.width4 = WIDTH / 2 + 4;
    i_block.thicc = 8;
    t_figure pp[5];
    initscr();
    keypad(stdscr, TRUE);
    raw();
    curs_set(0);

    start_color();

    while (ch != 27)
    {
        ch = getchar();
        playground();
        current_blocks = figure_printer(i_block, ch);
 
        refresh();
       
    }

    endwin();
}
