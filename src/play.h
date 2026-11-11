#ifndef PLAY_H
#define PLAY_H

#include "scores.h"
#include "settings.h"

/* Run one game. A qualifying result is added to hs and saved to disk. */
void play(const Settings *settings, HighScores *hs);

#endif
