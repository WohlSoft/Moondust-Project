QT += core
QT -= gui

TARGET = PGEFilelib_QT_test
CONFIG += console c++11
CONFIG -= app_bundle
INCLUDEPATH += ../../_common/

TEMPLATE = app
DEFINES += PGE_FILES_QT

include($$PWD/../../_common/PGE_File_Formats/File_FormatsQT.pri)

DESTDIR += $$PWD

SOURCES += main.cpp
