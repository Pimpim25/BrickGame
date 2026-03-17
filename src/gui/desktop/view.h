#ifndef WINDOW_H
#define WINDOW_H

#include <QTime>
#include <QTimer>
#include <QWidget>

#include "../../brick_game/tetris/specification.h"
#include "controller.h"

#define SPACE 32
#define PAUSE 32
#define ESCAPE 96
#define ENTER 10
#define DOWN 16777237
#define LEFT 16777234
#define RIGHT 16777236
#define UP 16777235

namespace my {
class View : public QWidget {
  Controller *controller;
  QTimer *tmr;

 public:
  static constexpr int cell = 30;
  explicit View(Controller *c, QWidget *parent = nullptr);
  ~View() noexcept override;

 protected:
  void keyPressEvent(QKeyEvent *event) override;
  void paintEvent(QPaintEvent *) override;
};

}  // namespace my
#endif