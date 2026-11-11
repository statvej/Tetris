CC ?= cc
CFLAGS ?= -O2
override CFLAGS += -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -Isrc

CORE = src/game.c src/storage.c src/settings.c src/scores.c
APP = src/main.c src/play.c src/ui.c
TESTS = tests/test_game tests/test_persistence

tetris: $(CORE) $(APP) $(wildcard src/*.h)
	$(CC) $(CFLAGS) -o $@ $(CORE) $(APP) -lncurses

tests/test_%: tests/test_%.c $(CORE) $(wildcard src/*.h) tests/check.h
	$(CC) $(CFLAGS) -Itests -o $@ $< $(CORE)

test: $(TESTS)
	@for t in $(TESTS); do ./$$t || exit 1; done

smoke: tetris
	python3 tests/smoke.py ./tetris

clean:
	rm -f tetris $(TESTS)

.PHONY: test smoke clean
