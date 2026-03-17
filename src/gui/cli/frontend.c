#include "frontend.h"

int main(void) {
  WIN_INIT();

  init_window_field();
  init_window_info();

  GameInfo_t Game = updateCurrentState();
  while (Game.field != NULL && Game.next != NULL) {
    int ch = GET_USER_INPUT;
    userInput(get_action(ch), 1);

    Game = updateCurrentState();
    if (Game.field != NULL && Game.next != NULL) {
      printf_field();
      print_info();
    }

    refresh();
    napms(10);
  }

  destroy_windows();
  endwin();
  return 0;
}

UserAction_t get_action(int user_input) {
  UserAction_t ch;
  if (user_input == KEY_UP)
    ch = Up;
  else if (user_input == KEY_DOWN)
    ch = Down;
  else if (user_input == KEY_LEFT)
    ch = Left;
  else if (user_input == KEY_RIGHT)
    ch = Right;
  else if (user_input == ESCAPE)
    ch = Terminate;
  else if (user_input == ENTER)
    ch = Start;
  else if (user_input == SPACE)
    ch = Action;
  else if (user_input == PAUSE)
    ch = Pause;
  return ch;
}

WINDOW *init_window_field() {
  static WINDOW *field_win = NULL;

  field_win = newwin(FIELD_LENGTH + 2, FIELD_WIDTH + 2, 0, 0);

  box(field_win, 0, 0);

  wrefresh(field_win);

  return field_win;
}

WINDOW *init_window_info() {
  static WINDOW *info_win = NULL;

  info_win = newwin(FIELD_LENGTH + 2, INFO_SIZE + 2, 0, FIELD_WIDTH + 3);

  box(info_win, 0, 0);

  wrefresh(info_win);

  return info_win;
}

void destroy_windows() {
  delwin(init_window_field());
  delwin(init_window_info());
}

void printf_field() {
  GameInfo_t Game = updateCurrentState();
  WINDOW *field_win = init_window_field();

  werase(field_win);
  box(field_win, 0, 0);

  for (int i = 0; i < FIELD_LENGTH; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      if (Game.field[i][j])
        mvwprintw(field_win, i + 1, j + 1, "#");
      else
        mvwprintw(field_win, i + 1, j + 1, " ");
    }
  }

  wrefresh(field_win);
}

void print_info() {
  GameInfo_t Game = updateCurrentState();
  WINDOW *info_win = init_window_info();

  werase(info_win);
  box(info_win, 0, 0);

  mvwprintw(info_win, 1, 2, "Level: %d", Game.level);
  mvwprintw(info_win, 3, 2, "Score: %d", Game.score);
  mvwprintw(info_win, 5, 2, "High score:");
  mvwprintw(info_win, 7, 2, "%d", Game.high_score);
  mvwprintw(info_win, 9, 2, "Speed: %d", Game.speed);

  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      if (Game.next[i][j])
        mvwprintw(info_win, i + 11, j + 7, "#");
      else
        mvwprintw(info_win, i + 11, j + 7, " ");
    }
  }

  wrefresh(info_win);
}