#pragma once

#include "specification.h"

#define FIGURE_COUNT 7
#define FIGURE_SIZE 4
#define FIELD_WIDTH 10
#define FIELD_LENGTH 20
#define START_X 3
#define START_Y 0

#define ONELINE 100
#define TWOLINES 300
#define THREELINES 700
#define FOURLINES 1500

#define WIN_INIT()         \
  {                        \
    initscr();             \
    noecho();              \
    cbreak();              \
    curs_set(0);           \
    keypad(stdscr, TRUE);  \
    nodelay(stdscr, TRUE); \
  }

typedef enum { START, MOVING, GAMEOVER } FSM;
typedef struct {
  int shapes[FIGURE_SIZE][FIGURE_SIZE];
  int figure_x;
  int figure_y;
  int prev_type;
  int next_type;
} Figures_struct;

typedef struct {
  int field[FIELD_LENGTH][FIELD_WIDTH];
  int next[FIGURE_SIZE][FIGURE_SIZE];
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
  FSM state;
  GameInfo_t *GameInfo;
  Figures_struct current_figure;
} GameInfo_t_extended;

GameInfo_t_extended *extendedGame();
struct timeval *start_tv();
struct timeval *end_tv();
GameInfo_t *Basic_Game();

void spawn(GameInfo_t_extended *extendedGame);
int move_figure(GameInfo_t_extended *extendedGame, int dx, int dy);
void drop_to_the_top(GameInfo_t_extended *Game);
void check_the_lines(GameInfo_t_extended *extendedGame);
void delete_the_line(GameInfo_t_extended *extendedGame, int str);
void rotate_figure(GameInfo_t_extended *extendedGame);
void remove_the_figure(GameInfo_t_extended *extendedGame);
void add_the_figure(GameInfo_t_extended *extendedGame);
void remove_basic_game(GameInfo_t *Game);
void read_high_score(GameInfo_t_extended *Game, const char *filename);
void put_high_score(GameInfo_t_extended *Game, const char *filename);
bool check_the_time(GameInfo_t_extended *Game);
void tick(GameInfo_t_extended *Game);

void on_start_case(UserAction_t action, GameInfo_t_extended *Game);
void on_moving_state(UserAction_t action, GameInfo_t_extended *Game);
void on_gameover_state(UserAction_t action, GameInfo_t_extended *Game);

void end_game();