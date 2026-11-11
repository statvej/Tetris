#include "check.h"
#include "scores.h"
#include "settings.h"
#include "storage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static ScoreEntry entry(const char *name, int score, long time_ms)
{
    ScoreEntry e = {.score = score, .lines = score / 100, .time_ms = time_ms};
    snprintf(e.name, NAME_LEN, "%s", name);
    return e;
}

static void test_storage_path(void)
{
    char buf[64];
    setenv("TETRIS_DATA_DIR", "/tmp/x", 1);
    CHECK(storage_path(buf, sizeof buf, "scores"));
    CHECK(strcmp(buf, "/tmp/x/.tetris_scores") == 0);
    CHECK(!storage_path(buf, 8, "scores")); /* does not fit */
}

static void test_settings_roundtrip(void)
{
    Settings s, loaded;
    settings_defaults(&s);
    s.mode = MODE_ULTRA;
    s.start_level = 7;
    s.ghost = false;
    s.theme = 1;
    CHECK(settings_save(&s));
    settings_load(&loaded);
    CHECK(loaded.mode == MODE_ULTRA && loaded.start_level == 7 && !loaded.ghost && loaded.theme == 1);
}

static void test_settings_corrupt(void)
{
    char path[512];
    CHECK(storage_path(path, sizeof path, "settings"));
    FILE *f = fopen(path, "w");
    CHECK(f);
    fputs("mode=99 level=-4 ghost=1 theme=50\n", f);
    fclose(f);

    Settings s;
    settings_load(&s);
    CHECK(s.mode == N_MODES - 1 && s.start_level == 1 && s.ghost && s.theme == N_THEMES - 1);

    f = fopen(path, "w");
    CHECK(f);
    fputs("garbage", f);
    fclose(f);
    settings_load(&s);
    CHECK(s.mode == MODE_MARATHON && s.start_level == 1 && s.ghost && s.theme == 0);
}

static void test_ranking(void)
{
    HighScores hs;
    memset(&hs, 0, sizeof hs);
    int scores[] = {500, 900, 100, 700, 300};
    for (int i = 0; i < TOP_N; i++)
    {
        ScoreEntry e = entry("AAA", scores[i], 0);
        CHECK(scores_insert(&hs, MODE_MARATHON, &e) >= 0);
    }
    CHECK(hs.count[MODE_MARATHON] == TOP_N);
    for (int i = 1; i < TOP_N; i++)
        CHECK(hs.entry[MODE_MARATHON][i - 1].score >= hs.entry[MODE_MARATHON][i].score);

    ScoreEntry low = entry("LOW", 50, 0);
    CHECK(scores_rank(&hs, MODE_MARATHON, &low) == -1);
    CHECK(scores_insert(&hs, MODE_MARATHON, &low) == -1);

    ScoreEntry top = entry("TOP", 1000, 0);
    CHECK(scores_insert(&hs, MODE_MARATHON, &top) == 0);
    CHECK(hs.count[MODE_MARATHON] == TOP_N);
    CHECK(hs.entry[MODE_MARATHON][TOP_N - 1].score == 300); /* 100 fell off */
}

static void test_sprint_ranks_by_time(void)
{
    HighScores hs;
    memset(&hs, 0, sizeof hs);
    ScoreEntry slow = entry("SLO", 0, 90000), fast = entry("FST", 0, 45000);
    CHECK(scores_insert(&hs, MODE_SPRINT, &slow) == 0);
    CHECK(scores_insert(&hs, MODE_SPRINT, &fast) == 0);
    CHECK(strcmp(hs.entry[MODE_SPRINT][0].name, "FST") == 0);
}

static void test_names_sanitized(void)
{
    HighScores hs;
    memset(&hs, 0, sizeof hs);
    ScoreEntry e = entry("a b", 10, 0);
    scores_insert(&hs, MODE_ULTRA, &e);
    CHECK(strcmp(hs.entry[MODE_ULTRA][0].name, "AB") == 0);
    e = entry("   ", 20, 0);
    scores_insert(&hs, MODE_ULTRA, &e);
    CHECK(strcmp(hs.entry[MODE_ULTRA][0].name, "---") == 0);
}

static void test_scores_roundtrip(void)
{
    HighScores hs, loaded;
    memset(&hs, 0, sizeof hs);
    ScoreEntry a = entry("ABC", 1234, 0), b = entry("XYZ", 4000, 0), c = entry("SPR", 0, 61000);
    scores_insert(&hs, MODE_MARATHON, &a);
    scores_insert(&hs, MODE_MARATHON, &b);
    scores_insert(&hs, MODE_SPRINT, &c);
    CHECK(scores_save(&hs));
    scores_load(&loaded);
    CHECK(loaded.count[MODE_MARATHON] == 2 && loaded.count[MODE_SPRINT] == 1 && loaded.count[MODE_ULTRA] == 0);
    CHECK(strcmp(loaded.entry[MODE_MARATHON][0].name, "XYZ") == 0);
    CHECK(loaded.entry[MODE_SPRINT][0].time_ms == 61000);
}

int main(void)
{
    char dir[] = "/tmp/tetris-test-XXXXXX";
    CHECK(mkdtemp(dir));
    setenv("TETRIS_DATA_DIR", dir, 1);

    test_storage_path();
    setenv("TETRIS_DATA_DIR", dir, 1);
    test_settings_roundtrip();
    test_settings_corrupt();
    test_ranking();
    test_sprint_ranks_by_time();
    test_names_sanitized();
    test_scores_roundtrip();

    char cmd[128];
    snprintf(cmd, sizeof cmd, "rm -rf %s", dir);
    if (system(cmd) != 0)
        return 1;
    puts("test_persistence OK");
    return 0;
}
