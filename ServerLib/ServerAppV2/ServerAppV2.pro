#-------------------------------------------------
#
# Project created by QtCreator 2015-06-24T22:05:14
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ServerAppV2
TEMPLATE = app


SOURCES += main.cpp\
    serverthread.cpp \
    PGEtcpserver.cpp \
    serverapplication.cpp

HEADERS  += \
    serverthread.h \
    PGEtcpserver.h \
    serverapplication.h

FORMS    += server.ui
