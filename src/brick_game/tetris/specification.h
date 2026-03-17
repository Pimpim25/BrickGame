#pragma once

#include <stdbool.h>

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

#ifdef __cplusplus
extern "C" {
#endif

// Функция `userInput` принимает на вход пользовательское действие `action` и
// дополнительный параметр `hold`, который отвечает за зажатие клавиши.
void userInput(UserAction_t action, bool hold);

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

// Функция `updateCurrentState` предназначена для получения данных для отрисовки
// в интерфейсе. Она возвращает структуру, содержащую информацию о текущем
// состоянии игры. Например, для тетриса истечение таймера приводит к смещению
// фигуры вниз на один ряд. Эта функция должна вызываться из интерфейса с
// некоторой периодичностью для поддержания интерфейса в актуальном состоянии.
GameInfo_t updateCurrentState();

#ifdef __cplusplus
}
#endif