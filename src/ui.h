#ifndef UI_H
#define UI_H

#include <stddef.h>

#include "game.h"
#include "scores.h"
#include "settings.h"

/* Start curses. Returns false (after restoring the terminal) if it is too small. */
bool ui_init(void);
void ui_shutdown(void);

/* Wait for a key; timeout_ms < 0 blocks. Returns a curses key code, or -1 on timeout. */
int ui_get_key(int timeout_ms);

void ui_draw_game(const Game *g, const Settings *s);
/* Overlay a message on the board; does not wait. */
void ui_banner(const char *text);

void ui_draw_menu(const char *const labels[], int count, int selected);
void ui_show_scores(const HighScores *hs);
void ui_show_text(const char *text);
/* Ask for up to three initials; the result is empty if the player just presses Enter. */
void ui_prompt_name(char out[NAME_LEN]);

/* Format milliseconds as m:ss.t */
void ui_format_time(char *buf, size_t size, long ms);

#endif
