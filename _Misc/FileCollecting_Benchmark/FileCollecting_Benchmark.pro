CONFIG -= qt
CONFIG += c++11

INCLUDEPATH += $$PWD/../../_common/

TARGET = FileCollecting_Benchmark
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

DESTDIR = $$PWD/bin

include($$PWD/../../_common/DirManager/dirman.pri)

SOURCES += main.cpp

