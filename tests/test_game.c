#include "check.h"
#include "game.h"

#include <stdlib.h>
#include <string.h>

static Game fresh(Mode mode)
{
    Game g;
    srand(1);
    game_init(&g, mode, 1);
    memset(g.board, 0, sizeof g.board);
    return g;
}

static void fill_row(Game *g, int r, int gap)
{
    for (int c = 0; c < BOARD_W; c++)
        g->board[r][c] = c == gap ? 0 : 1;
}

static void test_shapes(void)
{
    for (int t = 0; t < N_SHAPES; t++)
        for (int rot = 0; rot < 4; rot++)
        {
            Piece p = {t, rot, 0, 0};
            int n = shape_size(t), cnt = 0;
            for (int r = 0; r < n; r++)
                for (int c = 0; c < n; c++)
                    cnt += piece_cell(&p, r, c);
            CHECK(cnt == 4);
        }
    Piece i_vertical = {0, 1, 0, 0};
    int col = 0;
    for (int r = 0; r < 4; r++)
        col += piece_cell(&i_vertical, r, 2);
    CHECK(col == 4);
}

static void test_bag(void)
{
    Game g = fresh(MODE_MARATHON);
    for (int round = 0; round < 20; round++)
    {
        int seen[N_SHAPES] = {0};
        g.bag_pos = N_SHAPES; /* force a new bag; draw exactly one bag's worth */
        for (int i = 0; i < N_SHAPES; i++)
        {
            game_lock(&g); /* spawn() draws from the bag */
            g.finished = false;
            memset(g.board, 0, sizeof g.board);
            seen[g.bag[g.bag_pos - 1]]++;
        }
        for (int t = 0; t < N_SHAPES; t++)
            CHECK(seen[t] == 1);
    }
}

static void test_movement(void)
{
    Game g = fresh(MODE_MARATHON);
    g.cur = (Piece){1, 0, 0, 0};
    CHECK(!game_move(&g, 0, -1));
    g.cur.x = BOARD_W - 2;
    CHECK(!game_move(&g, 0, 1));
    g.cur.y = BOARD_H - 2;
    CHECK(!game_move(&g, 1, 0));
    g.cur = (Piece){0, 1, 0, BOARD_W - 3}; /* vertical I against the right wall */
    CHECK(game_rotate(&g, 1));
    g.cur = (Piece){2, 0, 5, 3};
    CHECK(game_rotate(&g, 1) && g.cur.rot == 1);
    CHECK(game_rotate(&g, -1) && g.cur.rot == 0);
    CHECK(game_rotate(&g, -1) && g.cur.rot == 3);
}

static void test_clear_and_score(void)
{
    Game g = fresh(MODE_MARATHON);
    g.next = 1;
    fill_row(&g, BOARD_H - 1, 4);
    fill_row(&g, BOARD_H - 2, 4);
    g.cur = (Piece){1, 0, BOARD_H - 2, 4}; /* O piece fills the gap in both rows */
    game_lock(&g);
    CHECK(g.lines == 2);
    CHECK(g.score == 300 + 1500); /* two lines, level 1, plus a perfect clear */
    CHECK(g.combo == 0);

    /* a lock that clears nothing breaks the combo */
    g.cur = (Piece){1, 0, BOARD_H - 2, 0};
    game_lock(&g);
    CHECK(g.combo == -1);
}

static void test_tetris_back_to_back(void)
{
    Game g = fresh(MODE_MARATHON);
    for (int round = 0; round < 2; round++)
    {
        g.next = 0;
        for (int r = BOARD_H - 4; r < BOARD_H; r++)
            fill_row(&g, r, 9);
        g.board[BOARD_H - 5][0] = 1; /* keep the board from being empty afterwards */
        g.cur = (Piece){0, 1, BOARD_H - 4, 7}; /* vertical I into the gap (column 9) */
        int before = g.score;
        game_lock(&g);
        CHECK(g.lines == 4 * (round + 1));
        if (round == 0)
            CHECK(g.score - before == 800);
        else
            CHECK(g.score - before == 800 + 400 + 50 * 1); /* b2b bonus + combo */
        g.finished = false;
    }
}

static void test_hold(void)
{
    Game g = fresh(MODE_MARATHON);
    int first = g.cur.type, upcoming = g.next;
    CHECK(game_hold(&g));
    CHECK(g.hold == first && g.cur.type == upcoming);
    CHECK(!game_hold(&g)); /* once per piece */
    game_hard_drop(&g);
    CHECK(game_hold(&g)); /* allowed again after the next piece spawns */
    CHECK(g.cur.type == first);
}

static void test_ghost_and_drops(void)
{
    Game g = fresh(MODE_MARATHON);
    g.cur = (Piece){1, 0, 0, 4};
    CHECK(game_ghost_y(&g) == BOARD_H - 2);
    g.board[10][4] = 1;
    CHECK(game_ghost_y(&g) == 8);
    int before = g.score;
    CHECK(game_soft_drop(&g));
    CHECK(g.score == before + 1);
    game_hard_drop(&g);
    CHECK(g.board[8 + 0][4] == 2); /* O piece landed on the block at row 10 */
}

static void test_modes(void)
{
    Game g = fresh(MODE_SPRINT);
    g.lines = SPRINT_LINES - 1;
    g.next = 1;
    fill_row(&g, BOARD_H - 1, 4);
    g.cur = (Piece){1, 0, BOARD_H - 2, 4};
    game_lock(&g);
    CHECK(g.finished && g.won);

    Game u = fresh(MODE_ULTRA);
    game_advance_time(&u, ULTRA_MS - 1);
    CHECK(!u.finished);
    game_advance_time(&u, 5);
    CHECK(u.finished && u.won && u.elapsed_ms == ULTRA_MS);

    Game m = fresh(MODE_MARATHON);
    game_advance_time(&m, 10 * ULTRA_MS);
    CHECK(!m.finished);
}

static void test_game_over(void)
{
    Game g = fresh(MODE_MARATHON);
    for (int r = 0; r < 4; r++)
        for (int c = 0; c < BOARD_W - 1; c++)
            g.board[r][c] = 1;
    g.cur = (Piece){1, 0, BOARD_H - 2, 0};
    game_lock(&g);
    CHECK(g.finished && !g.won);
    CHECK(!game_move(&g, 0, 1)); /* input is ignored once finished */
}

static void test_levels(void)
{
    Game g = fresh(MODE_MARATHON);
    CHECK(game_level(&g) == 1);
    g.lines = 25;
    CHECK(game_level(&g) == 3);
    g.lines = 10000;
    CHECK(game_level(&g) == MAX_LEVEL);
    CHECK(game_drop_delay_ms(&g) >= 60);
    game_init(&g, MODE_MARATHON, 99);
    CHECK(g.start_level == MAX_START_LEVEL);
}

int main(void)
{
    test_shapes();
    test_bag();
    test_movement();
    test_clear_and_score();
    test_tetris_back_to_back();
    test_hold();
    test_ghost_and_drops();
    test_modes();
    test_game_over();
    test_levels();
    puts("test_game OK");
    return 0;
}
