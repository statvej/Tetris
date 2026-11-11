#ifndef SCORES_H
#define SCORES_H

#include "game.h"

#define TOP_N 5
#define NAME_LEN 4 /* three characters plus the terminator */

typedef struct {
    char name[NAME_LEN];
    int score, lines;
    long time_ms;
} ScoreEntry;

typedef struct {
    ScoreEntry entry[N_MODES][TOP_N];
    int count[N_MODES];
} HighScores;

/* Sprint ranks by fastest time; the other modes by highest score. */
bool score_better(Mode mode, const ScoreEntry *a, const ScoreEntry *b);

/* Position the entry would take in the table, or -1 if it would not place. */
int scores_rank(const HighScores *hs, Mode mode, const ScoreEntry *e);
/* Insert the entry (name is sanitized to A-Z0-9); returns its rank or -1. */
int scores_insert(HighScores *hs, Mode mode, const ScoreEntry *e);

void scores_load(HighScores *hs);
bool scores_save(const HighScores *hs);

#endif
