#include "specification.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "backend.h"

GameInfo_t updateCurrentState() {
  GameInfo_t_extended *Game = extendedGame();
  if (check_the_time(Game)) tick(Game);
  add_the_figure(Game);
  GameInfo_t *basic_game = Basic_Game();

  if (basic_game->field && basic_game->next) {
    for (int i = 0; i < FIELD_LENGTH; i++) {
      for (int j = 0; j < FIELD_WIDTH; j++) {
        basic_game->field[i][j] = Game->field[i][j];
      }
    }

    for (int i = 0; i < FIGURE_SIZE; i++) {
      for (int j = 0; j < FIGURE_SIZE; j++) {
        basic_game->next[i][j] = Game->next[i][j];
      }
    }

    basic_game->score = Game->score;
    basic_game->high_score = Game->high_score;
    basic_game->level = Game->level;
    basic_game->speed = Game->speed;
    basic_game->pause = Game->pause;
  }

  remove_the_figure(Game);

  return *basic_game;
}

void userInput(UserAction_t action, bool hold) {
  GameInfo_t_extended *Game = extendedGame();

  if (hold) hold = false;
  if (action == Pause)
    Game->pause ^= 1;
  else if (action == Terminate) {
    end_game();
    Game->speed = -1;
  } else if (!Game->pause) {
    switch (Game->state) {
      case START:
        on_start_case(action, Game);
        break;
      case MOVING:
        on_moving_state(action, Game);
        break;
      case GAMEOVER:
        on_gameover_state(action, Game);
      default:
        break;
    }
  }
}
