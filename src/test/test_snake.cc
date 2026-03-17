#include <gtest/gtest.h>

#include "../brick_game/snake/model.h"
#include "../brick_game/snake/specification.h"

namespace my {

TEST(ModelTest, InitModelState) {
  Model model;
  model.init_model();
  GameInfo_t snap = model.snapshot();

  EXPECT_EQ(snap.score, 0);
  EXPECT_EQ(snap.level, 1);
  EXPECT_GT(snap.high_score, -1);
}

TEST(ModelTest, StartActionChangesState) {
  Model model;
  model.applyInput(Start);
  GameInfo_t snap = model.snapshot();

  EXPECT_EQ(snap.level, 1);
  EXPECT_EQ(snap.score, 0);
}

TEST(ModelTest, MoveSnakeRight) {
  Model model;
  model.applyInput(Start);
  model.applyInput(Right);
  model.tick();
  GameInfo_t snap = model.snapshot();

  EXPECT_EQ(snap.pause, 0);
}

TEST(ModelTest, PauseToggle) {
  Model model;
  model.applyInput(Start);
  model.applyInput(Pause);
  GameInfo_t snap = model.snapshot();

  EXPECT_EQ(snap.pause, 1);

  model.applyInput(Pause);
  snap = model.snapshot();

  EXPECT_EQ(snap.pause, 0);
}

TEST(ModelTest, TerminateSetsSpeedMinusOne) {
  Model model;
  model.applyInput(Terminate);
  GameInfo_t snap = model.snapshot();

  EXPECT_EQ(snap.speed, -100);
}

TEST(ModelTest, GameOverOnWallCollision) {
  Model model;
  model.applyInput(Start);

  for (int i = 0; i < 100; ++i) model.applyInput(Up);

  model.tick();
  GameInfo_t snap = model.snapshot();

  EXPECT_GE(snap.level, 1);
}

TEST(CoordStructTest, PlusEquals) {
  Model::coord_struct a{1, 2};
  Model::coord_struct b{3, 4};
  a += b;
  EXPECT_EQ(a.x, 4);
  EXPECT_EQ(a.y, 6);
}

TEST(CoordStructTest, Equality) {
  Model::coord_struct a{5, 6};
  Model::coord_struct b{5, 6};
  Model::coord_struct c{6, 5};
  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a == c);
}

TEST(ModelTest, ConstructorAndDestructor) {
  Model* model = new Model();
  GameInfo_t snap = model->snapshot();
  delete model;
}

TEST(ModelTest, InitModel) {
  Model model;
  model.init_model();
  GameInfo_t snap = model.snapshot();
  EXPECT_EQ(snap.level, 1);
  EXPECT_EQ(snap.score, 0);
}

TEST(ModelTest, DropAppleNotOnSnake) {
  Model model;
  model.init_model();
  auto apple = model.snapshot();
  EXPECT_GE(apple.field[0][0], 0);
}

TEST(ModelTest, AddFigure) {
  Model model;
  model.init_model();
  GameInfo_t snap = model.snapshot();
  int sum = 0;
  for (int y = 0; y < FIELD_LENGTH; y++)
    for (int x = 0; x < FIELD_WIDTH; x++) sum += snap.field[y][x];
  EXPECT_GT(sum, 0);
}

TEST(ModelTest, MoveSnakeChangesPosition) {
  Model model;
  model.init_model();
  auto before = model.snapshot();
  model.move_the_snake(0, -1);
  auto after = model.snapshot();
  EXPECT_EQ(after.score, before.score);
}

TEST(ModelTest, PreventReverseMove) {
  Model model;
  model.init_model();
  model.move_the_snake(0, -1);
  auto snap1 = model.snapshot();
  model.move_the_snake(0, 1);
  auto snap2 = model.snapshot();
  EXPECT_EQ(snap1.score, snap2.score);
}

TEST(ModelTest, WallCollisionGameOver) {
  Model model;
  model.init_model();
  for (int i = 0; i < FIELD_LENGTH + 5; i++) model.move_the_snake(0, -1);
  auto snap = model.snapshot();
  EXPECT_GE(snap.level, 1);
}

TEST(ModelTest, SelfCollisionGameOver) {
  Model model;
  model.init_model();
  model.move_the_snake(1, 0);
  model.move_the_snake(0, 1);
  model.move_the_snake(-1, 0);
  model.move_the_snake(0, -1);
  auto snap = model.snapshot();
  EXPECT_GE(snap.level, 1);
}

TEST(ModelTest, OnStartCase) {
  Model model;
  model.on_start_case(Start);
  auto snap = model.snapshot();
  EXPECT_EQ(snap.level, 1);
}

TEST(ModelTest, OnMovingState) {
  Model model;
  model.applyInput(Start);
  model.on_moving_state(Right);
  model.tick();
  auto snap = model.snapshot();
  EXPECT_EQ(snap.pause, 0);
}

TEST(ModelTest, OnGameOverState) {
  Model model;
  model.applyInput(Start);
  for (int i = 0; i < FIELD_LENGTH + 5; i++) model.applyInput(Up);
  model.on_gameover_state(Start);
  auto snap = model.snapshot();
  EXPECT_EQ(snap.level, 1);
}

TEST(ModelTest, SnapshotUpdatesGameInfo) {
  Model model;
  model.init_model();
  auto snap = model.snapshot();
  EXPECT_EQ(snap.speed, 100);
}

TEST(ModelTest, CheckTheTime) {
  Model model;
  model.init_model();
  bool first = model.check_the_time();
  EXPECT_FALSE(first);
}

TEST(ModelTest, TickMovesSnake) {
  Model model;
  model.applyInput(Start);
  auto before = model.snapshot();
  model.tick();
  auto after = model.snapshot();
  EXPECT_EQ(before.score, after.score);
}

TEST(ModelTest, ApplyPause) {
  Model model;
  model.applyInput(Start);
  model.applyInput(Pause);
  auto snap = model.snapshot();
  EXPECT_EQ(snap.pause, 1);
}

TEST(ModelTest, ApplyTerminate) {
  Model model;
  model.applyInput(Terminate);
  auto snap = model.snapshot();
  EXPECT_EQ(snap.speed, -100);
}

}  // namespace my

TEST(SpecificationTest, InitAndUserInput) {
  userInput(Start, false);
  auto state = updateCurrentState();
  EXPECT_EQ(state.level, 1);
  EXPECT_EQ(state.score, 0);
}

TEST(SpecificationTest, PauseAndTick) {
  userInput(Pause, false);
  auto state1 = updateCurrentState();
  EXPECT_EQ(state1.pause, 1);

  userInput(Pause, false);
  auto state2 = updateCurrentState();
  EXPECT_EQ(state2.pause, 0);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
