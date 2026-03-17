void printf_field(void) {}
void print_start(void) {}
void print_end(void) {}

#include <check.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "../brick_game/tetris/backend.h"  // подключаем прототипы и структуры

static struct timeval mock_start_val;
static struct timeval mock_end_val;

#define start_tv() (&mock_start_val)
#define end_tv() (&mock_end_val)

// START_TEST(test_check_the_time_false_immediate) {
//     GameInfo_t_extended Game = {0};
//     Game.speed = 100;

//     mock_start_val.tv_sec = 0;
//     mock_start_val.tv_usec = 0;

//     mock_end_val.tv_sec = 0;
//     mock_end_val.tv_usec = 50000;

//     bool status = check_the_time(&Game);
//     ck_assert(!status);
// }
// END_TEST

START_TEST(test_check_the_time_true_after_delay) {
  GameInfo_t_extended Game = {0};
  Game.speed = 100;

  mock_start_val.tv_sec = 0;
  mock_start_val.tv_usec = 0;

  mock_end_val.tv_sec = 0;
  mock_end_val.tv_usec = 150000;

  bool status = check_the_time(&Game);
  ck_assert(status);

  mock_end_val.tv_usec += 50000;
  status = check_the_time(&Game);
  ck_assert(!status);
}
END_TEST

START_TEST(spawn_test) {
  int shapes[FIGURE_COUNT][FIGURE_SIZE][FIGURE_SIZE] = {
      {{1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
      {{1, 0, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
      {{0, 0, 1, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
      {{0, 0, 0, 0}, {0, 1, 1, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}},
      {{0, 1, 1, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
      {{0, 1, 0, 0}, {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}},
      {{1, 1, 0, 0}, {0, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}}};

  GameInfo_t_extended Game = {0};
  Game.current_figure.prev_type = 10;
  Game.current_figure.next_type = 3;

  spawn(&Game);

  ck_assert_int_eq(Game.current_figure.prev_type, 3);

  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      ck_assert_int_eq(Game.current_figure.shapes[i][j], shapes[3][i][j]);
    }
  }

  ck_assert(Game.current_figure.next_type >= 0 &&
            Game.current_figure.next_type < FIGURE_COUNT);

  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      ck_assert_int_eq(Game.next[i][j],
                       shapes[Game.current_figure.next_type][i][j]);
    }
  }

  ck_assert_int_eq(Game.current_figure.figure_x, START_X);
  ck_assert_int_eq(Game.current_figure.figure_y, START_Y);
}
END_TEST

START_TEST(move_right_free_space) {
  GameInfo_t_extended Game = {0};

  Game.current_figure.figure_x = 5;
  Game.current_figure.figure_y = 5;
  Game.current_figure.shapes[0][0] = 1;
  Game.current_figure.shapes[0][1] = 1;
  Game.current_figure.shapes[1][0] = 1;
  Game.current_figure.shapes[1][1] = 1;

  move_figure(&Game, 1, 0);

  ck_assert_int_eq(Game.current_figure.figure_x, 6);
  ck_assert_int_eq(Game.current_figure.figure_y, 5);
}
END_TEST

START_TEST(move_left_wall_blocked) {
  GameInfo_t_extended Game = {0};

  Game.current_figure.figure_x = 0;
  Game.current_figure.figure_y = 5;
  Game.current_figure.shapes[0][0] = 1;

  move_figure(&Game, -1, 0);

  ck_assert_int_eq(Game.current_figure.figure_x, 0);
}
END_TEST

// START_TEST(move_down_gameover) {
//     GameInfo_t_extended Game = {0};

//     Game.current_figure.figure_x = 5;
//     Game.current_figure.figure_y = 0;
//     Game.current_figure.shapes[0][0] = 1;

//     Game.field[1][5] = 1;

//     move_figure(&Game, 0, 1);

//     ck_assert_int_eq(Game.state, GAMEOVER);
// }
// END_TEST

START_TEST(check_no_lines) {
  GameInfo_t_extended Game = {0};
  Game.level = 0;
  Game.speed = 1000;

  check_the_lines(&Game);

  ck_assert_int_eq(Game.score, 0);
  ck_assert_int_eq(Game.level, 0);
  ck_assert_int_eq(Game.speed, 1000);
}
END_TEST

START_TEST(check_two_lines) {
  GameInfo_t_extended Game = {0};

  for (int j = 0; j < FIELD_WIDTH; j++) {
    Game.field[FIELD_LENGTH - 1][j] = 1;
    Game.field[FIELD_LENGTH - 2][j] = 1;
  }

  check_the_lines(&Game);

  ck_assert_int_eq(Game.score, TWOLINES);
}
END_TEST

START_TEST(check_four_lines) {
  GameInfo_t_extended Game = {0};

  for (int i = FIELD_LENGTH - 1; i > FIELD_LENGTH - 5; i--) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      Game.field[i][j] = 1;
    }
  }

  check_the_lines(&Game);

  ck_assert_int_eq(Game.score, FOURLINES);
}
END_TEST

START_TEST(check_highscore_update) {
  GameInfo_t_extended Game = {0};

  for (int j = 0; j < FIELD_WIDTH; j++) {
    Game.field[FIELD_LENGTH - 1][j] = 1;
  }
  Game.high_score = 0;

  check_the_lines(&Game);

  ck_assert(Game.high_score == Game.score);
}
END_TEST

START_TEST(check_level_up) {
  GameInfo_t_extended Game = {0};
  Game.score = 590;
  Game.level = 0;
  Game.speed = 1000;

  for (int j = 0; j < FIELD_WIDTH; j++) {
    Game.field[FIELD_LENGTH - 1][j] = 1;
  }

  check_the_lines(&Game);

  ck_assert_int_eq(Game.level, 1);
  ck_assert_int_eq(Game.speed, 950);
}
END_TEST

START_TEST(add_and_remove_figure) {
  GameInfo_t_extended Game = {0};

  Game.current_figure.figure_x = 5;
  Game.current_figure.figure_y = 10;
  Game.current_figure.shapes[0][0] = 1;
  Game.current_figure.shapes[0][1] = 1;
  Game.current_figure.shapes[1][0] = 1;
  Game.current_figure.shapes[1][1] = 1;

  add_the_figure(&Game);

  ck_assert_int_eq(Game.field[10][5], 1);
  ck_assert_int_eq(Game.field[10][6], 1);
  ck_assert_int_eq(Game.field[11][5], 1);
  ck_assert_int_eq(Game.field[11][6], 1);

  remove_the_figure(&Game);

  ck_assert_int_eq(Game.field[10][5], 0);
  ck_assert_int_eq(Game.field[10][6], 0);
  ck_assert_int_eq(Game.field[11][5], 0);
  ck_assert_int_eq(Game.field[11][6], 0);
}
END_TEST

START_TEST(add_twice_remove_once) {
  GameInfo_t_extended Game = {0};

  Game.current_figure.figure_x = 2;
  Game.current_figure.figure_y = 3;
  Game.current_figure.shapes[0][0] = 1;

  add_the_figure(&Game);
  add_the_figure(&Game);

  ck_assert_int_eq(Game.field[3][2], 2);

  remove_the_figure(&Game);

  ck_assert_int_eq(Game.field[3][2], 1);
}
END_TEST

START_TEST(test_basic_game_allocation) {
  GameInfo_t *game = Basic_Game();
  ck_assert_ptr_nonnull(game);
  ck_assert_ptr_nonnull(game->field);
  ck_assert_ptr_nonnull(game->field[0]);
  ck_assert_ptr_nonnull(game->next);
  ck_assert_ptr_nonnull(game->next[0]);

  for (int i = 0; i < FIELD_LENGTH; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      ck_assert_int_eq(game->field[i][j], 0);
    }
  }

  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      ck_assert_int_eq(game->next[i][j], 0);
    }
  }

  ck_assert_int_eq(game->score, 0);
  ck_assert_int_eq(game->high_score, 0);
  ck_assert_int_eq(game->level, 1);
  ck_assert_int_eq(game->speed, 0);
  ck_assert_int_eq(game->pause, 0);

  remove_basic_game(game);
}
END_TEST

START_TEST(test_remove_basic_game_null) {
  remove_basic_game(NULL);
  ck_assert_msg(1, "remove_basic_game(NULL) не вызывает крэш");
}
END_TEST

START_TEST(test_read_high_score_missing_file) {
  GameInfo_t_extended Game = {0};
  remove("test_high_scores.txt");

  read_high_score(&Game, "test_high_scores.txt");
  ck_assert_int_eq(Game.high_score, 0);
}
END_TEST

START_TEST(test_put_and_read_high_score) {
  GameInfo_t_extended Game = {0};
  Game.score = 12345;
  Game.high_score = 0;

  put_high_score(&Game, "test_high_scores.txt");

  Game.high_score = 0;
  read_high_score(&Game, "test_high_scores.txt");

  ck_assert_int_eq(Game.high_score, 12345);

  remove("test_high_scores.txt");
}
END_TEST

START_TEST(test_on_start_case_other) {
  GameInfo_t_extended Game = {0};
  Game.state = START;

  on_start_case(Left, &Game);

  ck_assert_int_eq(Game.state, START);
}
END_TEST

START_TEST(test_on_moving_state_left) {
  GameInfo_t_extended Game = {0};
  spawn(&Game);
  int old_x = Game.current_figure.figure_x;

  on_moving_state(Left, &Game);
  ck_assert_int_eq(Game.current_figure.figure_x, old_x - 1);
}

END_TEST

START_TEST(test_on_moving_state_right) {
  GameInfo_t_extended Game = {0};
  spawn(&Game);
  int old_x = Game.current_figure.figure_x;

  on_moving_state(Right, &Game);
  ck_assert_int_eq(Game.current_figure.figure_x, old_x + 1);
}
END_TEST

START_TEST(test_extended_struct) {
  GameInfo_t_extended *game = extendedGame();
  ck_assert_int_eq(game->speed, 700);
}
END_TEST

START_TEST(test_drop) {
  GameInfo_t_extended *game = extendedGame();
  game->current_figure.figure_x = 5;
  game->current_figure.figure_y = 0;
  game->current_figure.shapes[0][0] = 1;

  game->field[1][5] = 1;

  drop_to_the_top(game);
}
END_TEST;

START_TEST(test_ucs) {
  GameInfo_t new_game = updateCurrentState();
  ck_assert_int_eq(new_game.speed, 700);
}
END_TEST;

START_TEST(test_UI_1) {
  GameInfo_t_extended *game = extendedGame();
  userInput(Start, 1);
  ck_assert_int_eq(game->state, MOVING);
}
END_TEST;

START_TEST(test_UI_2) {
  GameInfo_t_extended *game = extendedGame();
  userInput(Pause, 1);
  ck_assert_int_eq(game->pause, 1);
}
END_TEST;

START_TEST(test_UI_3) {
  GameInfo_t_extended *game = extendedGame();
  userInput(Start, 1);
  userInput(Left, 1);
  ck_assert_int_eq(game->state, MOVING);
}
END_TEST;

START_TEST(test_rotate_figure_normal_rotation) {
  GameInfo_t_extended Game = {0};

  int shape[FIGURE_SIZE][FIGURE_SIZE] = {
      {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

  memcpy(Game.current_figure.shapes, shape, sizeof(shape));
  Game.current_figure.prev_type = 1;
  Game.current_figure.figure_x = 3;
  Game.current_figure.figure_y = 5;

  rotate_figure(&Game);

  ck_assert_int_eq(Game.current_figure.shapes[0][0], 0);
  ck_assert_int_eq(Game.current_figure.shapes[1][0], 0);
  ck_assert_int_eq(Game.current_figure.shapes[2][0], 0);
  ck_assert_int_eq(Game.current_figure.shapes[0][2], 1);
}
END_TEST

START_TEST(test_rotate_figure_with_collision) {
  GameInfo_t_extended Game = {0};

  Game.current_figure.figure_x = 0;
  Game.current_figure.figure_y = FIELD_LENGTH - 2;
  Game.current_figure.prev_type = 1;

  int shape[FIGURE_SIZE][FIGURE_SIZE] = {
      {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

  memcpy(Game.current_figure.shapes, shape, sizeof(shape));
  Game.field[FIELD_LENGTH - 1][0] = 1;

  rotate_figure(&Game);

  ck_assert_mem_eq(Game.current_figure.shapes, shape, sizeof(shape));
}
END_TEST

START_TEST(test_rotate_figure_square_no_rotation) {
  GameInfo_t_extended Game = {0};

  Game.current_figure.prev_type = 3;
  int square[FIGURE_SIZE][FIGURE_SIZE] = {
      {1, 1, 0, 0}, {1, 1, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

  memcpy(Game.current_figure.shapes, square, sizeof(square));

  rotate_figure(&Game);

  ck_assert_mem_eq(Game.current_figure.shapes, square, sizeof(square));
}
END_TEST

START_TEST(test_rotate_figure_shift_out_of_bounds) {
  GameInfo_t_extended Game = {0};

  Game.current_figure.figure_x = FIELD_WIDTH - 1;
  Game.current_figure.prev_type = 1;

  int shape[FIGURE_SIZE][FIGURE_SIZE] = {
      {1, 1, 1, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0}};

  memcpy(Game.current_figure.shapes, shape, sizeof(shape));

  rotate_figure(&Game);

  ck_assert_int_lt(Game.current_figure.figure_x, FIELD_WIDTH - 1);
}
END_TEST

Suite *tetris_suite() {
  Suite *s;
  TCase *tc_core;

  s = suite_create("Tetris");
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_UI_1);
  tcase_add_test(tc_core, test_UI_2);
  tcase_add_test(tc_core, test_UI_3);
  tcase_add_test(tc_core, test_ucs);
  tcase_add_test(tc_core, spawn_test);
  tcase_add_test(tc_core, test_drop);
  tcase_add_test(tc_core, test_extended_struct);
  tcase_add_test(tc_core, move_right_free_space);
  tcase_add_test(tc_core, move_left_wall_blocked);

  tcase_add_test(tc_core, check_no_lines);
  tcase_add_test(tc_core, check_two_lines);
  tcase_add_test(tc_core, check_four_lines);
  tcase_add_test(tc_core, check_highscore_update);
  tcase_add_test(tc_core, check_level_up);

  tcase_add_test(tc_core, add_and_remove_figure);
  tcase_add_test(tc_core, add_twice_remove_once);

  tcase_add_test(tc_core, test_basic_game_allocation);
  tcase_add_test(tc_core, test_remove_basic_game_null);

  tcase_add_test(tc_core, test_read_high_score_missing_file);
  tcase_add_test(tc_core, test_put_and_read_high_score);

  tcase_add_test(tc_core, test_check_the_time_true_after_delay);

  tcase_add_test(tc_core, test_on_start_case_other);
  tcase_add_test(tc_core, test_on_moving_state_left);
  tcase_add_test(tc_core, test_on_moving_state_right);

  tcase_add_test(tc_core, test_rotate_figure_normal_rotation);
  tcase_add_test(tc_core, test_rotate_figure_with_collision);
  tcase_add_test(tc_core, test_rotate_figure_square_no_rotation);
  tcase_add_test(tc_core, test_rotate_figure_shift_out_of_bounds);

  suite_add_tcase(s, tc_core);
  return s;
}

int main() {
  Suite *s = tetris_suite();
  SRunner *sr = srunner_create(s);

  srunner_run_all(sr, CK_NORMAL);
  int number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);

  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
