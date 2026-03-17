#pragma once

#include <chrono>
#include <memory>
#include <vector>

#include "specification.h"

namespace my {

using namespace std::chrono;
#define INIT_SNAKE_SIZE 4
#define INIT_SNAKE_POS_Y 10
#define INIT_SNAKE_POS_X 5

class Model {
  typedef enum { START, MOVING, GAMEOVER } FSM;

 public:
  struct coord_struct {
    int x;
    int y;

    coord_struct& operator+=(const coord_struct& other);
    bool operator==(const coord_struct& other) const;
  };

 private:
  struct Figure_struct {
    coord_struct vector{};
    coord_struct snake_head{};
    std::vector<coord_struct> snake_tail{};
  };

  struct GameInfo_t_extended {
    int field[FIELD_LENGTH][FIELD_WIDTH];
    coord_struct apple_coord;
    int score;
    int high_score;
    int level;
    int speed;
    int pause;
    FSM state;
    Figure_struct snake;
    steady_clock::time_point start;
  };

  GameInfo_t_extended ExtendedGame{};

 public:
  Model() noexcept;
  GameInfo_t BasicGame{};

  void on_start_case(UserAction_t action) noexcept;
  void on_moving_state(UserAction_t action) noexcept;
  void on_gameover_state(UserAction_t action) noexcept;
  void init_model() noexcept;
  void pause_game() noexcept;
  void game_over() noexcept;

  void move_the_snake(int x, int y) noexcept;
  void drop_the_apple() noexcept;
  void add_the_figure() noexcept;
  void read_the_score() noexcept;
  void put_the_score() noexcept;
  bool check_the_time() noexcept;
  void tick() noexcept;

  GameInfo_t snapshot() noexcept;
  void applyInput(UserAction_t action) noexcept;

  ~Model() noexcept;
};

static std::unique_ptr<Model> gamePtr;

}  // namespace my