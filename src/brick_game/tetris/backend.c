#include "backend.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

GameInfo_t_extended *extendedGame() {
  static GameInfo_t_extended game = {0};

  if (!game.speed) {
    game.speed = 700.00;
    read_high_score(&game, "high_scores.txt");
  }

  start_tv();
  end_tv();

  return &game;
}

struct timeval *start_tv() {
  static struct timeval start_tv;

  return &start_tv;
}

struct timeval *end_tv() {
  static struct timeval end_tv;

  return &end_tv;
}

GameInfo_t *Basic_Game() {
  static GameInfo_t basic_game;
  GameInfo_t_extended *Game = extendedGame();

  if (Game->speed != -1) {
    if (!basic_game.field) {
      basic_game.field = malloc(sizeof(int *) * FIELD_LENGTH);
      if (basic_game.field) {
        int *data = calloc(FIELD_LENGTH * FIELD_WIDTH, sizeof(int));
        if (data) {
          for (int j = 0; j < FIELD_LENGTH; j++)
            basic_game.field[j] = data + j * FIELD_WIDTH;
        } else {
          free(basic_game.field);
          basic_game.field = NULL;
        }
      }
    }
    if (!basic_game.next) {
      basic_game.next = malloc(FIGURE_SIZE * sizeof(int *));
      if (basic_game.next) {
        int *ndata = calloc(FIGURE_SIZE * FIGURE_SIZE, sizeof(int));
        if (ndata) {
          for (int i = 0; i < FIGURE_SIZE; i++)
            basic_game.next[i] = ndata + i * FIGURE_SIZE;
        } else {
          free(basic_game.next);
          basic_game.next = NULL;
        }
      }
    }
    basic_game.score = 0;
    basic_game.high_score = 0;
    basic_game.level = 1;
    basic_game.speed = 0;
    basic_game.pause = 0;
  }

  return &basic_game;
}

void remove_basic_game(GameInfo_t *Game) {
  if (Game) {
    if (Game->field) {
      free(Game->field[0]);
      free(Game->field);
      Game->field = NULL;
    }
    if (Game->next) {
      free(Game->next[0]);
      free(Game->next);
      Game->next = NULL;
    }
  }
}

void spawn(GameInfo_t_extended *Game) {
  int shapes[FIGURE_COUNT][FIGURE_SIZE][FIGURE_SIZE] = {
      {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
      {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
      {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
      {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
      {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
      {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}};

  Game->current_figure.prev_type = Game->current_figure.next_type;

  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      Game->current_figure.shapes[i][j] =
          shapes[Game->current_figure.prev_type][i][j];
    }
  }

  Game->current_figure.next_type = rand() % 7;

  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      Game->next[i][j] = shapes[Game->current_figure.next_type][i][j];
    }
  }

  Game->current_figure.figure_x = START_X;
  Game->current_figure.figure_y = START_Y;
}

int move_figure(GameInfo_t_extended *Game, int dx, int dy) {
  int start_check = 0;
  // проверки по x
  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      if (dx == 1 && Game->current_figure.shapes[i][j] &&
          (Game->current_figure.figure_x + j == FIELD_WIDTH - 1 ||
           Game->field[Game->current_figure.figure_y + i]
                      [Game->current_figure.figure_x + j + dx]))
        dx = 0;
      if (dx == -1 && Game->current_figure.shapes[i][j] &&
          (Game->current_figure.figure_x + j == 0 ||
           Game->field[Game->current_figure.figure_y + i]
                      [Game->current_figure.figure_x + j + dx]))
        dx = 0;
    }
  }
  // проверки по y
  for (int i = FIGURE_SIZE - 1; i > -1; i--) {
    for (int j = FIGURE_SIZE - 1; j > -1; j--) {
      if (Game->current_figure.shapes[i][j] &&
          (Game->current_figure.figure_y + i == FIELD_LENGTH - 1 ||
           Game->field[Game->current_figure.figure_y + i + dy]
                      [Game->current_figure.figure_x + j])) {
        dy = 0;
        start_check++;
        add_the_figure(Game);
        check_the_lines(Game);
      }
    }
  }
  // проверка на заполненность поля
  for (int i = FIGURE_SIZE - 1; i > -1; i--) {
    for (int j = FIGURE_SIZE - 1; j > -1; j--) {
      if (Game->current_figure.shapes[i][j] &&
          Game->current_figure.figure_y + i == 0 &&
          Game->field[Game->current_figure.figure_y + i + 1]
                     [Game->current_figure.figure_x + j])
        Game->state = GAMEOVER;
    }
  }
  Game->current_figure.figure_x += dx;
  Game->current_figure.figure_y += dy;
  if (start_check) {
    spawn(Game);
  }

  return start_check;
}

void check_the_lines(GameInfo_t_extended *Game) {
  int check = 1, lines = 0, scores = 0;
  for (int i = FIELD_LENGTH - 1; i > -1; i--) {
    check = 1;
    for (int j = 0; j < FIELD_WIDTH - 1; j++) {
      if (!Game->field[i][j]) check = 0;
    }
    if (check) {
      delete_the_line(Game, i);
      i++;
      lines++;
    }
  }
  if (lines == 1)
    scores = ONELINE;
  else if (lines == 2)
    scores = TWOLINES;
  else if (lines == 3)
    scores = THREELINES;
  else if (lines >= 4)
    scores = FOURLINES;

  Game->score += scores;
  if (Game->score > Game->high_score) {
    Game->high_score = Game->score;
    put_high_score(Game, "high_scores.txt");
  }

  if ((Game->score / 600 > Game->level) && Game->level < 10) {
    Game->level += 1;
    Game->speed -= 50;
  }
}

void delete_the_line(GameInfo_t_extended *Game, int str) {
  for (int i = str; i > -1; i--) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      if (i > 0) {
        Game->field[i][j] = Game->field[i - 1][j];
      } else {
        Game->field[i][j] = 0;
      }
    }
  }
}

void rotate_figure(GameInfo_t_extended *Game) {
  int temp[FIGURE_SIZE][FIGURE_SIZE] = {0}, size = 4, check_ix = 0,
      check_jx = 0, check = 0;
  if (Game->current_figure.prev_type == 0)
    size = 4;
  else if (Game->current_figure.prev_type == 3)
    size = 2;
  else
    size = 3;

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      temp[j][size - 1 - i] = Game->current_figure.shapes[i][j];
    }
  }
  for (int i = FIGURE_SIZE - 1; i > -1; i--) {
    for (int j = FIGURE_SIZE - 1; j > -1; j--) {
      if (Game->current_figure.shapes[i][j] &&
          (Game->current_figure.figure_y + i == FIELD_LENGTH - 1 ||
           Game->field[Game->current_figure.figure_y + i + 1]
                      [Game->current_figure.figure_x + j])) {
        check++;
      }
    }
  }
  if (!check && size != 2) {
    for (int i = 0; i < FIGURE_SIZE; i++) {
      for (int j = 0; j < FIGURE_SIZE; j++) {
        Game->current_figure.shapes[i][j] = temp[i][j];
      }
    }
  }
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      if (Game->current_figure.figure_x + j == FIELD_WIDTH && !check_ix) {
        Game->current_figure.figure_x--;
        check_ix++;
        i = 0;
        j = 0;
      } else if (Game->current_figure.figure_x + j == 1 && !check_jx) {
        Game->current_figure.figure_x++;
        check_jx++;
        i = 0;
        j = 0;
      }
    }
  }
}

void remove_the_figure(GameInfo_t_extended *Game) {
  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      if (Game->current_figure.shapes[i][j])
        Game->field[Game->current_figure.figure_y + i]
                   [Game->current_figure.figure_x + j]--;
    }
  }
}

void add_the_figure(GameInfo_t_extended *Game) {
  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      if (Game->current_figure.shapes[i][j])
        Game->field[Game->current_figure.figure_y + i]
                   [Game->current_figure.figure_x + j]++;
    }
  }
}

void read_high_score(GameInfo_t_extended *Game, const char *filename) {
  Game->high_score = 0;
  char str[256] = {0};

  FILE *fp = fopen(filename, "r");
  if (fp) {
    if (fgets(str, sizeof(str), fp)) {
      sscanf(str, "%d", &(Game->high_score));
    }
    fclose(fp);
  }
}

void put_high_score(GameInfo_t_extended *Game, const char *filename) {
  FILE *fp = fopen(filename, "w");
  if (fp) {
    fprintf(fp, "%d", Game->score);
    fclose(fp);
  }
}

bool check_the_time(GameInfo_t_extended *Game) {
  bool status = 0;
  long long start_us, end_us;
  struct timeval *start = start_tv();
  struct timeval *end = end_tv();

  gettimeofday(end, NULL);
  start_us = start->tv_sec * 1000000 + start->tv_usec;
  end_us = end->tv_sec * 1000000 + end->tv_usec;

  if ((double)(end_us - start_us) / 1000.0 >= Game->speed) {
    status = 1;
    *start = *end;
  }

  return status;
}

void tick(GameInfo_t_extended *Game) {
  if (Game->pause) return;
  if (Game->state == MOVING) {
    move_figure(Game, 0, 1);
  }
}

void on_start_case(UserAction_t action, GameInfo_t_extended *Game) {
  switch (action) {
    case Start:
      memset(Game, 0, sizeof(GameInfo_t_extended));
      spawn(Game);
      Game->state = MOVING;
      break;
    default:
      break;
  }
}

void on_moving_state(UserAction_t action, GameInfo_t_extended *Game) {
  switch (action) {
    case Left:
      move_figure(Game, -1, 0);
      break;
    case Right:
      move_figure(Game, 1, 0);
      break;
    case Down:
      drop_to_the_top(Game);
      break;
    case Action:
      rotate_figure(Game);
      break;
    case Start:
      on_start_case(action, Game);
      break;
    default:
      break;
  }
}

void drop_to_the_top(GameInfo_t_extended *Game) {
  while (move_figure(Game, 0, 1) < 1) {
  }
}

void end_game() { remove_basic_game(Basic_Game()); }

void on_gameover_state(UserAction_t action, GameInfo_t_extended *Game) {
  for (int i = 0; i < FIELD_LENGTH; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      Game->field[i][j] = 1;
    }
  }
  switch (action) {
    case Start:
      on_start_case(action, Game);
      break;
    default:
      break;
  }
}
