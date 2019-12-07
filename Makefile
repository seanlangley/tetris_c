default:
	gcc src/tetris.c -Wall -Wextra -Werror -g -lncurses -lpthread -o ./bin/tetris
clean:
	rm -rf ./bin/*

