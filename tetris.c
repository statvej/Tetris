#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#define WIDTH 40
#define HEIGHT 20
// #define DROP_SPEED 1000000
int position = 0;
int position_up = 0;
int time_index = 0;
int fig_count = 0;
int level_arr[10] = {0, 10000, 90000, 70000, 80000, 70000, 6000, 50000, 40000, 3000};

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
    int high;

    int bottom[4];
} t_figure;

typedef struct s_bottom_blocks_coord
{
    int x[100];
    int y[100];
    t_figure bottom_figs[100];
} t_bottom_blocks_coord;

t_figure current_figure;
t_bottom_blocks_coord bottom_blocks;

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

            mvaddch(count_h, count_w, ' ');
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

bool botom_colision_checker(t_figure check){
if()

return FALSE;
}

int figure_printer(t_figure block, int input, int level)
{
    //color pairs
    init_pair(1, COLOR_YELLOW, COLOR_YELLOW); //For O-Block
    init_pair(2, COLOR_CYAN, COLOR_CYAN);     // for I-BLOCK
    if (time_index >= level)
    {
        time_index = 0;
        if (position_up < HEIGHT - 2)
        {
            ++position_up;
        }
        else if(botom_colision_checker(block)){
            
            return 0;
        }
        else
        {
            position_up = HEIGHT - 2;
            return 0;
        }
    }

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

    block.height1 = block.height1 + position_up - block.high + 1;
    block.height2 = block.height2 + position_up - block.high + 1;
    block.height3 = block.height3 + position_up - block.high + 1;
    block.height4 = block.height4 + position_up - block.high + 1;

    attron(COLOR_PAIR(block.color_pair)); //printing fase
    mvprintw(block.height1, block.width1, "  ");
    mvprintw(block.height2, block.width2, "  ");
    mvprintw(block.height3, block.width3, "  ");
    mvprintw(block.height4, block.width4, "  ");
    attroff(COLOR_PAIR(block.color_pair));
    return 1;
}
void bottom_adder(t_figure tetromino)
{
    bottom_blocks.bottom_figs[fig_count] = tetromino;
    bottom_blocks.x[fig_count] = position;
    bottom_blocks.y[fig_count] = position_up;
    ++fig_count;
}
void bottom_printer()
{
    int count = 0;
    //color pairs
    init_pair(1, COLOR_YELLOW, COLOR_YELLOW); //For O-Block
    init_pair(2, COLOR_CYAN, COLOR_CYAN);     // for I-BLOCK
    while (count < fig_count)
    {
        attron(COLOR_PAIR(bottom_blocks.bottom_figs[count].color_pair)); //printing fase

        mvprintw(bottom_blocks.bottom_figs[count].height1 + bottom_blocks.y[count] - bottom_blocks.bottom_figs[count].high + 1, bottom_blocks.bottom_figs[count].width1 + bottom_blocks.x[count] - 1, "  ");
        mvprintw(bottom_blocks.bottom_figs[count].height2 + bottom_blocks.y[count] - bottom_blocks.bottom_figs[count].high + 1, bottom_blocks.bottom_figs[count].width2 + bottom_blocks.x[count] - 1, "  ");
        mvprintw(bottom_blocks.bottom_figs[count].height3 + bottom_blocks.y[count] - bottom_blocks.bottom_figs[count].high + 1, bottom_blocks.bottom_figs[count].width3 + bottom_blocks.x[count] - 1, "  ");
        mvprintw(bottom_blocks.bottom_figs[count].height4 + bottom_blocks.y[count] - bottom_blocks.bottom_figs[count].high + 1, bottom_blocks.bottom_figs[count].width4 + bottom_blocks.x[count] - 1, "  ");

        attroff(COLOR_PAIR(bottom_blocks.bottom_figs[count].color_pair));
        ++count;
    }
}
t_figure rnd_figure()
{
    t_figure null_block; //NULL-BLOCK for error returning

    t_figure o_block; //O-BLOCK parameters     /* COLOR PAIR IS ALSO BLOCK NUMBER IN RANDOMIZER */
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
    o_block.high = 2;
    o_block.bottom = {0, 0};
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
    i_block.high = 1;
    int random = rand() % 2;

    switch (random)
    {
    case 0:
        return o_block;
        break;
    case 1:
        return i_block;
    default:
        break;
    }
    return null_block;
}
int main()
{
    int ch;
    int random_checker;

    int level = 9;
    t_figure temp;

    current_figure = rnd_figure();
    initscr();
    keypad(stdscr, TRUE);
    raw();
    curs_set(0);
    nodelay(stdscr, TRUE);
    start_color();

    while (ch != 27)
    {
        int time_counter = 0;
        ch = getch();

        playground();
        random_checker = figure_printer(current_figure, ch, level_arr[level]);
        if (random_checker != 1)
        {
            bottom_adder(current_figure);
            current_figure = rnd_figure();
            position_up = 0;
            position = 0;
            random_checker = figure_printer(current_figure, ch, level_arr[level]);
        }
        bottom_printer();
       
        while (time_counter <= 1)
        {
            usleep(10);
            ++time_index;
            ++time_counter;
        }
        refresh();
        bottom_printer();
    }
    endwin();
    return fig_count;
}
