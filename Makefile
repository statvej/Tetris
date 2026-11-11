CC ?= cc
CFLAGS ?= -O2
override CFLAGS += -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -Isrc

CORE = src/game.c
TESTS = tests/test_game

tetris: tetris.c
	$(CC) $(CFLAGS) -o $@ $< -lncurses

tests/test_%: tests/test_%.c $(CORE) tests/check.h
	$(CC) $(CFLAGS) -Itests -o $@ $< $(CORE)

test: $(TESTS)
	@for t in $(TESTS); do ./$$t || exit 1; done

clean:
	rm -f tetris $(TESTS)

.PHONY: test clean
