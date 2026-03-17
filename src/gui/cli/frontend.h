#pragma once
#include <ncurses.h>

#define SPACE 32
#define PAUSE 'e'
#define ESCAPE 27
#define ENTER 10
#define DOWN 2
#define LEFT 4
#define RIGHT 5

#define INFO_SIZE 15
#define FIGURE_SIZE 4
#define FIELD_WIDTH 10
#define FIELD_LENGTH 20

#define WIN_INIT()         \
  {                        \
    initscr();             \
    noecho();              \
    cbreak();              \
    curs_set(0);           \
    keypad(stdscr, TRUE);  \
    nodelay(stdscr, TRUE); \
  }

// #include "../../brick_game/tetris/backend.h"
// #include "../../brick_game/tetris/specification.h"

// #include "../../brick_game/tetris/backend.h"
// #include "../../brick_game/snake/specification.h"

// export LD_LIBRARY_PATH=.

#define GET_USER_INPUT getch()

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

extern GameInfo_t updateCurrentState();
extern void userInput(UserAction_t action, bool hold);
UserAction_t get_action(int user_input);

void printf_field();
void print_info();

WINDOW *init_window_field();
WINDOW *init_window_info();
void destroy_windows();