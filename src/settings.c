#include "settings.h"

#include "storage.h"

#include <stdio.h>

static const char *const THEME_NAMES[N_THEMES] = {"Color", "Mono"};

const char *theme_name(int theme)
{
    return theme >= 0 && theme < N_THEMES ? THEME_NAMES[theme] : "?";
}

void settings_defaults(Settings *s)
{
    *s = (Settings){MODE_MARATHON, 1, true, 0};
}

static int clamp(int v, int lo, int hi)
{
    return v < lo ? lo : v > hi ? hi : v;
}

void settings_load(Settings *s)
{
    char path[512];
    int mode, level, ghost, theme;

    settings_defaults(s);
    if (!storage_path(path, sizeof path, "settings"))
        return;
    FILE *f = fopen(path, "r");
    if (!f)
        return;
    if (fscanf(f, "mode=%d level=%d ghost=%d theme=%d", &mode, &level, &ghost, &theme) == 4)
    {
        s->mode = clamp(mode, 0, N_MODES - 1);
        s->start_level = clamp(level, 1, MAX_START_LEVEL);
        s->ghost = ghost != 0;
        s->theme = clamp(theme, 0, N_THEMES - 1);
    }
    fclose(f);
}

bool settings_save(const Settings *s)
{
    char path[512];
    if (!storage_path(path, sizeof path, "settings"))
        return false;
    FILE *f = fopen(path, "w");
    if (!f)
        return false;
    fprintf(f, "mode=%d level=%d ghost=%d theme=%d\n", s->mode, s->start_level, s->ghost, s->theme);
    return fclose(f) == 0;
}
