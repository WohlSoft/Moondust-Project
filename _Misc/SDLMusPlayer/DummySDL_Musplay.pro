#-------------------------------------------------
#
# Project created by QtCreator 2014-09-19T21:03:41
#
#-------------------------------------------------

QT       += core gui widgets

CONFIG += static

macx: QMAKE_CXXFLAGS += -Wno-header-guard
!macx: {
QMAKE_CXXFLAGS += -static -static-libgcc
QMAKE_LFLAGS += -Wl,-rpath=\'\$\$ORIGIN\'
}

TARGET = DummySDL_Musplay
TEMPLATE = app
DESTDIR = $$PWD/../../bin

CONFIG += c++11
CONFIG += thread

LIBS += -lSDL2 -lSDL2_mixer_ext
win32: LIBS += -lSDL2main
win32: LIBS += libversion
win32: static: {
    QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++
    LIBS += -lvorbisfile -lvorbis -lmikmod -lmad -lflac -logg
}

INCLUDEPATH += $$PWD/../../_Libs/_builds/linux/include
LIBS += -L$$PWD/../../_Libs/_builds/linux/lib

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui


