TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += ../../_common/
DESTDIR += $$PWD

DEFINES -= PGE_FILES_QT

include($$PWD/../../_common/PGE_File_Formats/File_FormatsSTL.pri)

SOURCES += main.cpp

HEADERS += \
    dirent/dirent.h
