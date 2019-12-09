default:
	gcc src/tetris.c -Wall -Wextra -Werror -g -lncurses -o ./bin/tetris
clean:
	rm -rf ./bin/*

