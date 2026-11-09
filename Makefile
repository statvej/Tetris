tetris: tetris.c
	$(CC) -Wall -Wextra -O2 -o $@ $< -lncurses

clean:
	rm -f tetris

.PHONY: clean
