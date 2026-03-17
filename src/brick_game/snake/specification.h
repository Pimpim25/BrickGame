#pragma once

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

#define FIELD_WIDTH 10
#define FIELD_LENGTH 20
#define FIGURE_SIZE 4

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  int **field;
  int **next;  // заглушка
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

void init_if_needed();
void userInput(UserAction_t action, bool hold);
GameInfo_t updateCurrentState();

#ifdef __cplusplus
}
#endif
