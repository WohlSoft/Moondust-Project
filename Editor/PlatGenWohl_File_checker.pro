#-------------------------------------------------
#
# Project created by QtCreator 2014-03-18T16:09:15
#
#-------------------------------------------------

QT       += core gui

TARGET = PlatGenWohl_File_checker
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    aboutdialog.cpp

HEADERS  += mainwindow.h \
    aboutdialog.h

FORMS    += mainwindow.ui \
    aboutdialog.ui

RC_FILE = platgenw.rc

OTHER_FILES += \
    images/saveas.png \
    images/save.png \
    images/open.png \
    images/new.png

RESOURCES += \
    editor.qrc
