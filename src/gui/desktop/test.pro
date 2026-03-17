QT += widgets gui

SOURCES += main.cpp view.cpp controller.cpp

CONFIG += c++20

unix:!macx:LIBS += -lXtst -lX11

LIBS += -L../../build -lgame

QMAKE_RPATHDIR += ../../build
