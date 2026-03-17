#include "model.h"

#include <fstream>
namespace my {

Model::coord_struct& Model::coord_struct::operator+=(
    const coord_struct& other) {
  x += other.x;
  y += other.y;
  return *this;
}

bool Model::coord_struct::operator==(const coord_struct& other) const {
  return (x == other.x && y == other.y);
}

Model::Model() noexcept {
  BasicGame.field = new int*[FIELD_LENGTH]();
  for (size_t i = 0; i < FIELD_LENGTH; i++) {
    BasicGame.field[i] = new int[FIELD_WIDTH]();
  }
  BasicGame.next = new int*[FIGURE_SIZE]();
  for (size_t i = 0; i < FIGURE_SIZE; i++) {
    BasicGame.next[i] = new int[FIGURE_SIZE]();
  }
}

void Model::init_model() noexcept {
  ExtendedGame.snake.snake_head = {INIT_SNAKE_POS_X, INIT_SNAKE_POS_Y};
  ExtendedGame.snake.snake_tail.clear();
  ExtendedGame.snake.snake_tail.push_back(ExtendedGame.snake.snake_head);

  for (int i = 1; i < INIT_SNAKE_SIZE; i++) {
    ExtendedGame.snake.snake_tail.push_back(
        {ExtendedGame.snake.snake_head.x, ExtendedGame.snake.snake_head.y + i});
  }

  ExtendedGame.snake.vector = {0, -1};
  ExtendedGame.speed = 1;
  ExtendedGame.level = 1;

  drop_the_apple();
  read_the_score();
  ExtendedGame.start = steady_clock::now();
}

Model::~Model() noexcept {
  if (BasicGame.field) game_over();
}

void Model::drop_the_apple() noexcept {
  bool flag{true};
  while (flag) {
    flag = false;
    ExtendedGame.apple_coord = {rand() % FIELD_WIDTH, rand() % FIELD_LENGTH};
    for (const auto& it : ExtendedGame.snake.snake_tail)
      if (it == ExtendedGame.apple_coord) flag = true;
  }
}

void Model::add_the_figure() noexcept {
  if (BasicGame.field) {
    for (int y = 0; y < FIELD_LENGTH; y++)
      for (int x = 0; x < FIELD_WIDTH; x++) BasicGame.field[y][x] = 0;

    for (const auto& it : ExtendedGame.snake.snake_tail) {
      if (it.x >= 0 && it.x < FIELD_WIDTH && it.y >= 0 && it.y < FIELD_LENGTH)
        BasicGame.field[it.y][it.x] = 1;
    }

    BasicGame.field[ExtendedGame.apple_coord.y][ExtendedGame.apple_coord.x] = 1;
  }
}

void Model::move_the_snake(int x, int y) noexcept {
  int prev_x = ExtendedGame.snake.vector.x;
  int prev_y = ExtendedGame.snake.vector.y;

  if (!(x != 0 && y != 0)) {
    if (x == -prev_x && y == -prev_y) return;
  }

  ExtendedGame.snake.vector.x = x;
  ExtendedGame.snake.vector.y = y;

  ExtendedGame.snake.snake_head.x += ExtendedGame.snake.vector.x;
  ExtendedGame.snake.snake_head.y += ExtendedGame.snake.vector.y;

  if (ExtendedGame.snake.snake_head == ExtendedGame.apple_coord) {
    drop_the_apple();
    ExtendedGame.score++;
    if (ExtendedGame.high_score < ExtendedGame.score) {
      ExtendedGame.high_score = ExtendedGame.score;
      put_the_score();
    }
    if (ExtendedGame.score % 5 == 0 &&
        ExtendedGame.score / 5 >= ExtendedGame.level && ExtendedGame.level < 10)
      ExtendedGame.level++;
  } else {
    if (ExtendedGame.snake.snake_head.x < 0 ||
        ExtendedGame.snake.snake_head.x >= FIELD_WIDTH ||
        ExtendedGame.snake.snake_head.y < 0 ||
        ExtendedGame.snake.snake_head.y >= FIELD_LENGTH) {
      ExtendedGame.state = GAMEOVER;
      return;
    }

    for (auto it = ExtendedGame.snake.snake_tail.begin() + 1;
         it < ExtendedGame.snake.snake_tail.end(); ++it) {
      if (*it == ExtendedGame.snake.snake_head) {
        ExtendedGame.state = GAMEOVER;
        return;
      }
    }
    ExtendedGame.snake.snake_tail.pop_back();
  }

  auto it = ExtendedGame.snake.snake_tail.begin();
  ExtendedGame.snake.snake_tail.insert(it, ExtendedGame.snake.snake_head);

  if (ExtendedGame.snake.snake_tail.size() == 200)
    ExtendedGame.state = GAMEOVER;
}

void Model::read_the_score() noexcept {
  std::ifstream ifs("../../brick_game/snake/score.dat",
                    std::ios::in | std::ios::binary);
  if (ifs.is_open())
    ifs.read((char*)&ExtendedGame.high_score, sizeof(ExtendedGame.high_score));
  ifs.close();
}

void Model::put_the_score() noexcept {
  std::ofstream ofs("../../brick_game/snake/score.dat",
                    std::ios::out | std::ios::binary);
  if (ofs.is_open())
    ofs.write((char*)&ExtendedGame.high_score, sizeof(ExtendedGame.high_score));
  ofs.close();
}

void Model::on_start_case(UserAction_t action) noexcept {
  switch (action) {
    case Start:
      ExtendedGame = GameInfo_t_extended{};
      init_model();
      ExtendedGame.state = MOVING;
      break;
    default:
      break;
  }
}

void Model::on_moving_state(UserAction_t action) noexcept {
  switch (action) {
    case Left:
      move_the_snake(-1, 0);
      break;
    case Right:
      move_the_snake(1, 0);
      break;
    case Up:
      move_the_snake(0, -1);
      break;
    case Down:
      move_the_snake(0, 1);
      break;
    case Action:
      move_the_snake(ExtendedGame.snake.vector.x, ExtendedGame.snake.vector.y);
      break;
  }
}

void Model::on_gameover_state(UserAction_t action) noexcept {
  for (int i = 0; i < FIELD_LENGTH; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      BasicGame.field[i][j] = 1;
    }
  }
  switch (action) {
    case Start:
      on_start_case(action);
      break;
    default:
      break;
  }
}

void Model::game_over() noexcept {
  for (size_t i = 0; i < FIELD_LENGTH; i++) {
    delete[] BasicGame.field[i];
    BasicGame.field[i] = NULL;
  }
  delete[] BasicGame.field;
  BasicGame.field = NULL;

  for (size_t i = 0; i < FIGURE_SIZE; i++) {
    delete[] BasicGame.next[i];
    BasicGame.next[i] = NULL;
  }
  delete[] BasicGame.next;
  BasicGame.next = NULL;
}

GameInfo_t Model::snapshot() noexcept {
  if (BasicGame.field) {
    for (int y = 0; y < FIELD_LENGTH; y++)
      for (int x = 0; x < FIELD_WIDTH; x++) BasicGame.field[y][x] = 0;

    add_the_figure();
  }

  BasicGame.score = ExtendedGame.score;
  BasicGame.high_score = ExtendedGame.high_score;
  BasicGame.level = ExtendedGame.level;
  BasicGame.speed = 100 * ExtendedGame.speed;
  BasicGame.pause = ExtendedGame.pause;

  return BasicGame;
}

bool Model::check_the_time() noexcept {
  auto temp = 50 * (10 - ExtendedGame.level);

  if (steady_clock::now() > ExtendedGame.start + milliseconds(temp)) {
    ExtendedGame.start = steady_clock::now();
    return true;
  }
  return false;
}

void Model::tick() noexcept {
  if (ExtendedGame.pause) return;
  if (ExtendedGame.state == MOVING) {
    move_the_snake(ExtendedGame.snake.vector.x, ExtendedGame.snake.vector.y);
  }
}

void Model::applyInput(UserAction_t action) noexcept {
  if (action == Pause)
    ExtendedGame.pause ^= 1;
  else if (action == Terminate) {
    game_over();
    ExtendedGame.speed = -1;
  } else if (!ExtendedGame.pause) {
    switch (ExtendedGame.state) {
      case START:
        on_start_case(action);
        break;
      case MOVING:
        on_moving_state(action);
        break;
      case GAMEOVER:
        on_gameover_state(action);
      default:
        break;
    }
  }
}

};  // namespace my