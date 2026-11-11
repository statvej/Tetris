#ifndef SETTINGS_H
#define SETTINGS_H

#include "game.h"

#define N_THEMES 2

typedef struct {
    Mode mode;
    int start_level; /* 1..MAX_START_LEVEL */
    bool ghost;
    int theme; /* 0 = color, 1 = monochrome */
} Settings;

const char *theme_name(int theme);

void settings_defaults(Settings *s);
/* Missing or corrupt files fall back to defaults; loaded values are clamped. */
void settings_load(Settings *s);
bool settings_save(const Settings *s);

#endif
