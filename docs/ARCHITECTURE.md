# Architecture

The code is split so that the rules of the game never touch the terminal.

```
src/
  game.[ch]      rules engine: pieces, board, bag, hold, scoring, modes (no I/O)
  settings.[ch]  persisted menu options
  scores.[ch]    top-5 high score tables per mode
  storage.[ch]   where data files live
  ui.[ch]        every ncurses call
  play.[ch]      one game: input, timing, recording the result
  main.c         menu loop
tests/
  test_game.c         rules engine unit tests
  test_persistence.c  settings, scores and path tests
  smoke.py            end-to-end test through a pty
```

## Dependency direction

```
main -> play -> ui -> game
  |       |      |
  |       +------+--> scores, settings -> storage
  +-----------------> settings, scores
```

`game`, `settings`, `scores` and `storage` have no curses dependency, so the tests link only those four files.

## Game state

`Game` is a plain struct holding a 20x10 board of shape ids (0 is empty), the current piece, the next piece, the hold slot, the 7-bag and the counters. Pieces are `{type, rotation, y, x}`; a shape is an n x n string and rotation is computed rather than stored, so adding a piece means adding one line to `SHAPES`.

All state changes go through `game_move`, `game_rotate`, `game_hold`, `game_soft_drop`, `game_hard_drop`, `game_tick` and `game_advance_time`. Once `finished` is set they become no-ops, and `won` tells the caller whether the mode's objective was reached (Sprint cleared, Ultra timer ran out).

## Timing

`play.c` owns the clock. Each frame it waits for a key for at most the time left until the next gravity step, applies the key, tells the game how much time passed, and steps gravity if due. Pause resets both timestamps so paused time does not count.

## Persistence

Both files are tiny text formats. Loading never trusts the file: settings are clamped and scores are re-ranked on insert.

| File | Format |
| --- | --- |
| `.tetris_settings` | `mode=0 level=1 ghost=1 theme=0` |
| `.tetris_scores` | one line per entry: `mode name score lines time_ms` |

## Adding a game mode

1. Add the value to `Mode` in `game.h` and a name in `MODE_NAMES`.
2. Add its win/finish condition to `game_lock` or `game_advance_time`.
3. Decide how it ranks in `score_better` and whether a result qualifies in `record_result`.
4. Add a test to `tests/test_game.c`.
