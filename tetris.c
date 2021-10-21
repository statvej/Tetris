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

// typedef struct s_coord
// {
//     int x[19];
//     int y[19];
// } t_coord;

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
    int x[1000];
    int y[1000];
    t_figure bottom_figs[1000];
} t_bottom_blocks_coord;
// t_coord colision;
int relief_y[38];
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

// void get_relief()
// {
//     int count_w = 0;
//     int count_h = 0;
//     int ch = ' ';
//     while (count_w < 38)
//     {

//         while (ch == ' ')
//         {
//             ch = mvinch(count_h, count_w + 1);
//             if (ch == 'O')
//             {

//                 relief_y[count_w] = count_h - 2;
//                 exit(0);
//             }
//             else if (ch == '_')
//             {

//                 relief_y[count_w] = count_h - 1;
//             }

//             count_h++;
//             relief_y[count_w] = count_h - 2;
//         }

//         count_w++;
//     }
// }

int get_relief(int count_w)
{

    int count_h = 19;
    int ch = mvinch(count_h, count_w);
    if (ch == ' ')
    {
        relief_y[count_w] = 19;
    }
    else
    {
        while (ch > 250)
        {
            count_h = count_h - 1;
            ch = mvinch(count_h, count_w + 1);
        }
    }

    return count_h;
}

int colision()
{
    int x1 = current_figure.width1 + position - 1;
    int x2 = current_figure.width2 + position - 1;
    int x3 = current_figure.width3 + position - 1;
    int x4 = current_figure.width4 + position - 1;
    int y1 = current_figure.height1 + position_up - current_figure.high + 1;
    int y2 = current_figure.height2 + position_up - current_figure.high + 1;
    int y3 = current_figure.height3 + position_up - current_figure.high + 1;
    int y4 = current_figure.height4 + position_up - current_figure.high + 1;
    if (y1 == get_relief(x1) + 1)
    {
        return 1;
    }
    if (y1 == get_relief(x2) + 1)
    {
        return 1;
    }
    if (y1 == get_relief(x3) + 1)
    {
        return 1;
    }
    if (y1 == get_relief(x4) + 1)
    {
        return 1;
    }

    if (y2 == get_relief(x1) + 1)
    {
        return 1;
    }
    if (y2 == get_relief(x2) + 1)
    {
        return 1;
    }
    if (y2 == get_relief(x3) + 1)
    {
        return 1;
    }
    if (y2 == get_relief(x4) + 1)
    {
        return 1;
    }

    if (y3 == get_relief(x1) + 1)
    {
        return 1;
    }
    if (y3 == get_relief(x2) + 1)
    {
        return 1;
    }
    if (y3 == get_relief(x3) + 1)
    {
        return 1;
    }
    if (y3 == get_relief(x4) + 1)
    {
        return 1;
    }

    if (y4 == get_relief(x1) + 1)
    {
        return 1;
    }
    if (y4 == get_relief(x2) + 1)
    {
        return 1;
    }
    if (y4 == get_relief(x3) + 1)
    {
        return 1;
    }
    if (y4 == get_relief(x4) + 1)
    {
        return 1;
    }

    return 0;
}

int figure_printer(t_figure block, int input, int level, int colision_check)
{
    //mvprintw(10, 20, "%d", colision());
    if (colision_check == 1)
    {
        // position_up = position_up - 2;
        // exit(1);
        return 0;
    }
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
    case 's':
        if (position_up < HEIGHT - 2)
        {
            position_up++;
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
    mvprintw(block.height1, block.width1, "@@");
    mvprintw(block.height2, block.width2, "@@");
    mvprintw(block.height3, block.width3, "@@");
    mvprintw(block.height4, block.width4, "@@");
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

        mvprintw(bottom_blocks.bottom_figs[count].height1 + bottom_blocks.y[count] - bottom_blocks.bottom_figs[count].high + 1, bottom_blocks.bottom_figs[count].width1 + bottom_blocks.x[count] - 1, "66");
        mvprintw(bottom_blocks.bottom_figs[count].height2 + bottom_blocks.y[count] - bottom_blocks.bottom_figs[count].high + 1, bottom_blocks.bottom_figs[count].width2 + bottom_blocks.x[count] - 1, "66");
        mvprintw(bottom_blocks.bottom_figs[count].height3 + bottom_blocks.y[count] - bottom_blocks.bottom_figs[count].high + 1, bottom_blocks.bottom_figs[count].width3 + bottom_blocks.x[count] - 1, "66");
        mvprintw(bottom_blocks.bottom_figs[count].height4 + bottom_blocks.y[count] - bottom_blocks.bottom_figs[count].high + 1, bottom_blocks.bottom_figs[count].width4 + bottom_blocks.x[count] - 1, "66");

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

    int level = 1;
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
        int check = colision();
        playground();

        mvprintw(20, 20, "%d", colision());
        random_checker = figure_printer(current_figure, ch, level_arr[level], check);
        if (random_checker != 1)
        {
            bottom_adder(current_figure);

            current_figure = rnd_figure();
            position_up = 0;
            position = 0;
            random_checker = figure_printer(current_figure, ch, level_arr[level], check);
        }
        bottom_printer();

        while (time_counter <= 1)
        {
            usleep(10);
            ++time_index;
            ++time_counter;
        }
        refresh();
    }
    endwin();
    return fig_count;
}

