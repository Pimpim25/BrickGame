#include <QApplication>
#include <QWidget>

#include "view.h"

int main(int argc, char **argv) {
  QApplication app(argc, argv);

  my::Controller controller;
  my::View view(&controller);

  view.show();

  return app.exec();
}