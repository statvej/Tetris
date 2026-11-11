#include "scores.h"

#include "storage.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

bool score_better(Mode mode, const ScoreEntry *a, const ScoreEntry *b)
{
    return mode == MODE_SPRINT ? a->time_ms < b->time_ms : a->score > b->score;
}

int scores_rank(const HighScores *hs, Mode mode, const ScoreEntry *e)
{
    int n = hs->count[mode], pos = 0;
    while (pos < n && !score_better(mode, e, &hs->entry[mode][pos]))
        pos++;
    return pos < TOP_N ? pos : -1;
}

static void sanitize_name(char out[NAME_LEN], const char *in)
{
    int len = 0;
    for (; *in && len < NAME_LEN - 1; in++)
        if (isalnum((unsigned char)*in))
            out[len++] = (char)toupper((unsigned char)*in);
    if (len == 0)
        for (; len < NAME_LEN - 1; len++)
            out[len] = '-';
    out[len] = '\0';
}

int scores_insert(HighScores *hs, Mode mode, const ScoreEntry *e)
{
    int pos = scores_rank(hs, mode, e);
    if (pos < 0)
        return -1;
    int n = hs->count[mode];
    int last = n < TOP_N ? n : TOP_N - 1; /* the slot that gets overwritten or appended */
    for (int i = last; i > pos; i--)
        hs->entry[mode][i] = hs->entry[mode][i - 1];
    hs->entry[mode][pos] = *e;
    sanitize_name(hs->entry[mode][pos].name, e->name);
    if (n < TOP_N)
        hs->count[mode] = n + 1;
    return pos;
}

void scores_load(HighScores *hs)
{
    char path[512];
    memset(hs, 0, sizeof *hs);
    if (!storage_path(path, sizeof path, "scores"))
        return;
    FILE *f = fopen(path, "r");
    if (!f)
        return;

    int mode;
    ScoreEntry e;
    while (fscanf(f, "%d %3s %d %d %ld", &mode, e.name, &e.score, &e.lines, &e.time_ms) == 5)
    {
        if (mode < 0 || mode >= N_MODES || hs->count[mode] >= TOP_N)
            continue;
        /* re-insert instead of trusting the file's ordering */
        scores_insert(hs, (Mode)mode, &e);
    }
    fclose(f);
}

bool scores_save(const HighScores *hs)
{
    char path[512];
    if (!storage_path(path, sizeof path, "scores"))
        return false;
    FILE *f = fopen(path, "w");
    if (!f)
        return false;
    for (int m = 0; m < N_MODES; m++)
        for (int i = 0; i < hs->count[m]; i++)
        {
            const ScoreEntry *e = &hs->entry[m][i];
            fprintf(f, "%d %s %d %d %ld\n", m, e->name, e->score, e->lines, e->time_ms);
        }
    return fclose(f) == 0;
}
