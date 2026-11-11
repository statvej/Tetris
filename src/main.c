#include "play.h"
#include "scores.h"
#include "settings.h"
#include "ui.h"

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

enum { M_START, M_MODE, M_LEVEL, M_GHOST, M_THEME, M_SCORES, M_CREDITS, M_QUIT, M_COUNT };

/* Change the setting behind a menu item; items that are not settings are ignored. */
static void adjust(Settings *s, int item, int d)
{
    switch (item)
    {
    case M_MODE:
        s->mode = (Mode)((s->mode + d + N_MODES) % N_MODES);
        break;
    case M_LEVEL:
        s->start_level = (s->start_level - 1 + d + MAX_START_LEVEL) % MAX_START_LEVEL + 1;
        break;
    case M_GHOST:
        s->ghost = !s->ghost;
        break;
    case M_THEME:
        s->theme = (s->theme + d + N_THEMES) % N_THEMES;
        break;
    }
}

static void build_labels(const Settings *s, char text[M_COUNT][32], const char *labels[M_COUNT])
{
    snprintf(text[M_START], 32, "Start");
    snprintf(text[M_MODE], 32, "< Mode: %s >", mode_name(s->mode));
    snprintf(text[M_LEVEL], 32, "< Level: %d >", s->start_level);
    snprintf(text[M_GHOST], 32, "< Ghost: %s >", s->ghost ? "on" : "off");
    snprintf(text[M_THEME], 32, "< Theme: %s >", theme_name(s->theme));
    snprintf(text[M_SCORES], 32, "High scores");
    snprintf(text[M_CREDITS], 32, "Credits");
    snprintf(text[M_QUIT], 32, "Quit");
    for (int i = 0; i < M_COUNT; i++)
        labels[i] = text[i];
}

int main(void)
{
    Settings settings;
    HighScores scores;
    int sel = M_START;

    srand((unsigned)time(NULL));
    settings_load(&settings);
    scores_load(&scores);
    if (!ui_init())
        return 1;

    for (;;)
    {
        char text[M_COUNT][32];
        const char *labels[M_COUNT];
        build_labels(&settings, text, labels);
        ui_draw_menu(labels, M_COUNT, sel);

        switch (ui_get_key(-1))
        {
        case KEY_UP:
        case 'w':
            sel = (sel + M_COUNT - 1) % M_COUNT;
            break;
        case KEY_DOWN:
        case 's':
            sel = (sel + 1) % M_COUNT;
            break;
        case KEY_LEFT:
            adjust(&settings, sel, -1);
            break;
        case KEY_RIGHT:
            adjust(&settings, sel, 1);
            break;
        case 10:
        case KEY_ENTER:
            if (sel == M_START)
            {
                settings_save(&settings);
                play(&settings, &scores);
            }
            else if (sel == M_SCORES)
                ui_show_scores(&scores);
            else if (sel == M_CREDITS)
                ui_show_text("yarutiunov, fstaryk, hpinchuktr");
            else if (sel == M_QUIT)
                goto done;
            else
                adjust(&settings, sel, 1);
            break;
        case 'q':
        case 27:
            goto done;
        }
    }
done:
    settings_save(&settings);
    ui_shutdown();
    return 0;
}
