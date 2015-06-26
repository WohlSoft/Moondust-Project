#-------------------------------------------------
#
# Project created by QtCreator 2015-06-25T18:09:41
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ServerAppV2ClientSide
TEMPLATE = app


SOURCES += main.cpp\
        clientapplication.cpp \
    clientconnection.cpp

HEADERS  += clientapplication.h \
    clientconnection.h

FORMS    += clientapplication.ui
