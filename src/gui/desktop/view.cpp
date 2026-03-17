#include "view.h"

#include <QDebug>
#include <QKeyEvent>
#include <QPainter>
namespace my {
View::View(Controller *c, QWidget *parent) : QWidget(parent), controller(c) {
  setFocusPolicy(Qt::StrongFocus);
  setFixedSize(600, 600);  // 300 600

  tmr = new QTimer(this);
  connect(tmr, &QTimer::timeout, this, [this]() { update(); });
  tmr->start(16);
}

View::~View() noexcept = default;

void View::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
    case Qt::Key_Up:
      controller->apply_input(Up);
      break;
    case Qt::Key_Down:
      controller->apply_input(Down);
      break;
    case Qt::Key_Left:
      controller->apply_input(Right);
      break;
    case Qt::Key_Right:
      controller->apply_input(Left);
      break;
    case Qt::Key_Space:
      controller->apply_input(Action);
      break;
    case Qt::Key_Escape:
      controller->apply_input(Terminate);
      close();
      break;
    case Qt::Key_Return:
      controller->apply_input(Start);
      break;
    case Qt::Key_P:
      controller->apply_input(Pause);
      break;
    default:
      QWidget::keyPressEvent(event);
  }
}

void View::paintEvent(QPaintEvent *) {
  GameInfo_t Game = updateCurrentState();

  QPainter painter(this);

  QFont levelFont("Monospace", 20);
  levelFont.setStyleHint(QFont::TypeWriter);
  painter.setFont(levelFont);

  painter.drawText(350, 40, "Level:");
  painter.drawText(475, 40, QString::number(Game.level));

  painter.drawText(350, 80, "Score:");
  painter.drawText(475, 80, QString::number(Game.score));

  painter.drawText(350, 120, "High score:");
  painter.drawText(370, 150, QString::number(Game.high_score));

  painter.drawText(350, 190, "Speed:");
  painter.drawText(475, 190, QString::number(Game.speed));

  painter.translate(QPoint(300, 0));
  painter.rotate(90);

  painter.fillRect(0, 0, 600, 300, QBrush{"#5379e0"});

  for (int i = 0; i < FIELD_LENGTH; i++) {
    for (int j = 0; j < FIELD_WIDTH; j++) {
      if (Game.field[i][j]) {
        painter.fillRect(View::cell * i, View::cell * j, View::cell, View::cell,
                         QBrush{"#ee83a8"});
      }
    }
  }

  for (int i = 0; i < FIGURE_SIZE; i++) {
    for (int j = 0; j < FIGURE_SIZE; j++) {
      if (Game.next[i][j]) {
        painter.fillRect(300 + View::cell * i, View::cell * j - 200, View::cell,
                         View::cell, QBrush{"#ee83a8"});
      }
    }
  }
}

}  // namespace my