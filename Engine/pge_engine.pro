#-------------------------------------------------
#
# Project created by QtCreator 2014-09-05T22:51:01
#
#-------------------------------------------------

QT += core gui
#QT += widgets

TARGET = pge_engine
TEMPLATE = app


SOURCES += main.cpp

HEADERS  +=

LIBS += -lSDL2
win32: LIBS += -lSDL2main
win32: LIBS += libversion
unix:  LIBS += -lglut -lGLU

FORMS    +=
