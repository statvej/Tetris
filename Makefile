all:
	clang tetris.c -lncurses -o tetris
reinstall:
	rm -rf tetris
	clang tetris.c -lncurses -o tetris