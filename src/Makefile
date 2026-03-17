CC := gcc
CP := g++
CFLAGS := -std=c11 -Werror -Wall -Wextra
CPFLAGS := -std=c++20
LIB_FLAGS = $(shell pkg-config --libs check)

.PHONY: all libtetris.so libsnake.so install game_cli uninstall clean dvi dist test gcov_report clang clean leaks snake

all: tetris.so

libtetris.so:
	mkdir -p build/
	$(CC) -c -fPIC $(CFLAGS) brick_game/tetris/*.c
	$(CC) -shared -o build/libgame.so *.o
	mv *.o build/

libsnake.so:
	mkdir -p build/
	$(CP) -c -fPIC $(CPFLAGS) brick_game/snake/*.cpp
	$(CP) -shared -o build/libgame.so *.o
	mv *.o build/

game_cli:
	$(CC) $(CFLAGS) gui/cli/frontend.c -o build/game -Lbuild -lgame -lncurses -Wl,-rpath,'$$ORIGIN'
	cd build && ./game

game_desktop:
	(cd gui/desktop && make clean)
	(cd gui/desktop && make)
	(cd gui/desktop && qmake)
	(cd gui/desktop && ./test)

install:
	$(CC) $(CFLAGS) gui/cli/frontend.c -o game -Lbuild -lgame -lncurses
	cp ./game install_dir/

uninstall:
	rm -f ./install_dir/game
	rm -f ./install_dir/*.o
	rm -f ./install_dir/*.so

dist:
	mkdir -p dist/BrickGame
	cp -rp brick_game gui test Makefile install_dir dist/BrickGame
	tar -czf dist/BrickGame.tar.gz -C dist BrickGame

test:
# 	$(CC) $(CFLAGS) brick_game/tetris/backend.c brick_game/tetris/specification.c ./test/test_tetris.c $(LIB_FLAGS) -o ./test/tetris_test -lncurses -lm
# 	./test/tetris_test

	$(CP) $(CPFLAGS) -g brick_game/snake/model.cpp brick_game/snake/specification.cpp ./test/test_snake.cc -o ./test/snake_test -lgtest
	./test/snake_test

gcov_report:
# 	$(CC) --coverage $(CFLAGS) brick_game/tetris/backend.c brick_game/tetris/specification.c ./test/test.c $(LIB_FLAGS) -o ./test/TEST -lncurses -lm
# 	./test/TEST
	$(CP) --coverage -O0 -g $(CPFLAGS) brick_game/snake/model.cpp brick_game/snake/specification.cpp ./test/test_snake.cc -o ./test/snake_test -lgtest -lpthread
	./test/snake_test
	lcov --ignore-errors mismatch -t "melody" -o melody.info -c -d ./test
	genhtml -o ./test/coverage_report melody.info

leaks:
# 	$(CC) $(CFLAGS) brick_game/tetris/backend.c brick_game/tetris/specification.c ./test/test.c $(LIB_FLAGS) -o ./test/TEST -lncurses -lm
# 	./test/TEST
	$(CP) $(CPFLAGS) -g brick_game/snake/model.cpp brick_game/snake/specification.cpp ./test/test_snake.cc -o ./test/snake_test -lgtest
	./test/snake_test
	valgrind --tool=memcheck --leak-check=yes ./test/snake_test

clang:
	mv ../materials/linters/.clang-format ../src
	clang-format -i ./brick_game/tetris/*.c ./brick_game/tetris/*.h ./gui/cli/*.c ./gui/cli/*.h ./test/*.c ./test/*.cc ./brick_game/snake/*.cpp ./brick_game/snake/*.h ./gui/desktop/*.cpp ./gui/desktop/*.h
	mv .clang-format ../materials/linters/

clean:
	rm -rf ./test/coverage_report/
	rm -rf ./doxygen
	rm -f ./test/TEST*
	rm -f ./test/TEST
	rm -f ./main
	rm -f *.info
	rm -rf ./dist
	rm -rf ./build/*.o
	rm -rf ./build/game
	rm -rf ./build/*.so
	rm -rf ./gui/desktop/*.o
	rm -rf ./test/*.gcda
	rm -rf ./test/*.gcno