# Tetris

A terminal Tetris written in C with ncurses.

## Build and run

Requires a C compiler and ncurses (`libncurses-dev` on Debian/Ubuntu, included with macOS).

```bash
make
./tetris
```

The terminal needs to be at least 50x24.

## Controls

| Key | Action |
| --- | --- |
| Left / Right, A / D | Move |
| Up, W, X | Rotate clockwise |
| Z | Rotate counter-clockwise |
| Down, S | Soft drop (1 point per row) |
| Space | Hard drop (2 points per row) |
| C, H | Hold the current piece (once per piece) |
| P | Pause (any key resumes) |
| Q, Esc | Quit to the menu |

## Game modes

- **Marathon**: endless; the level goes up every 10 lines.
- **Sprint**: clear 40 lines as fast as you can. Ranked by time.
- **Ultra**: score as much as you can in 2 minutes.

## Scoring

Lines cleared are worth 100 / 300 / 500 / 800 for 1 / 2 / 3 / 4 lines, multiplied by the level. On top of that:

- **Combo**: +50 x combo x level for each consecutive clear.
- **Back-to-back**: a tetris right after another tetris gets +50%.
- **Perfect clear**: +1500 x level for emptying the board.

## Menu settings

Mode, starting level (1-9), ghost piece on/off and theme (color / mono) are changed with Left/Right (or Enter) and saved between runs. The top five results per mode are kept in the high score table.

Settings and scores are stored in `$TETRIS_DATA_DIR`, falling back to `$HOME`, then the current directory, as `.tetris_settings` and `.tetris_scores`.

## Development

```bash
make test    # unit tests for the game rules, scoring, settings and high scores
make smoke   # drives the real binary through a pseudo-terminal (needs python3)
```

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for how the code is laid out.
