#include "play.h"

#include "ui.h"

#include <ncurses.h>
#include <time.h>

static long now_ms(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000L + t.tv_nsec / 1000000;
}

/* Returns false when the player quits. */
static bool run(Game *g, const Settings *s)
{
    long last_tick = now_ms(), prev = last_tick;

    while (!g->finished)
    {
        ui_draw_game(g, s);
        long wait = last_tick + game_drop_delay_ms(g) - now_ms();

        switch (ui_get_key(wait > 0 ? (int)wait : 0))
        {
        case KEY_LEFT:
        case 'a':
            game_move(g, 0, -1);
            break;
        case KEY_RIGHT:
        case 'd':
            game_move(g, 0, 1);
            break;
        case KEY_UP:
        case 'w':
        case 'x':
            game_rotate(g, 1);
            break;
        case 'z':
            game_rotate(g, -1);
            break;
        case KEY_DOWN:
        case 's':
            game_soft_drop(g);
            break;
        case ' ':
            game_hard_drop(g);
            last_tick = now_ms();
            break;
        case 'c':
        case 'h':
            game_hold(g);
            break;
        case 'p':
            ui_draw_game(g, s);
            ui_banner("PAUSED");
            ui_get_key(-1);
            prev = last_tick = now_ms();
            break;
        case 'q':
        case 27:
            return false;
        }

        long now = now_ms();
        game_advance_time(g, now - prev);
        prev = now;
        if (!g->finished && now - last_tick >= game_drop_delay_ms(g))
        {
            last_tick = now;
            game_tick(g);
        }
    }
    return true;
}

static void record_result(const Game *g, HighScores *hs)
{
    bool eligible = g->mode == MODE_SPRINT ? g->won : g->score > 0;
    ScoreEntry e = {.score = g->score, .lines = g->lines, .time_ms = g->elapsed_ms};

    if (!eligible || scores_rank(hs, g->mode, &e) < 0)
        return;
    ui_prompt_name(e.name);
    scores_insert(hs, g->mode, &e);
    scores_save(hs);
    ui_show_scores(hs);
}

void play(const Settings *s, HighScores *hs)
{
    Game g;
    game_init(&g, s->mode, s->start_level);
    if (!run(&g, s))
        return;

    ui_draw_game(&g, s);
    ui_banner(g.mode == MODE_SPRINT && g.won ? "SPRINT CLEAR!" : g.won ? "TIME UP!" : "GAME OVER");
    ui_get_key(1000); /* swallow keys mashed during the final drop... */
    ui_get_key(-1);   /* ...then wait for a deliberate press */
    record_result(&g, hs);
}
