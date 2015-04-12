#-------------------------------------------------
#
# Project created by QtCreator 2015-04-12T18:04:18
#
#-------------------------------------------------

QT       -= gui

TARGET = ServerAPI
TEMPLATE = lib

CONFIG += staticlib
CONFIG += c++11

SOURCES += \
    packet.cpp \
    pgesocket.cpp

HEADERS += \
    packet.h \
    pgesocket.h \
    pgesocketdefines.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
