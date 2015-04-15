#-------------------------------------------------
#
# Project created by QtCreator 2015-03-31T19:17:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32: {
QT += winextras
} else {
QT -= winextras
}

android:{
DESTDIR = $$PWD/../bin/_android
} else {
DESTDIR = $$PWD/../bin
}

TARGET = pge_manager
TEMPLATE = app

CONFIG += c++11

CONFIG += static
CONFIG += thread

!macx: {
QMAKE_CXXFLAGS += -static -static-libgcc
QMAKE_LFLAGS += -Wl,-rpath=\'\$\$ORIGIN\'
}

android:{
    release:OBJECTS_DIR = ../bin/_build/_android/manager/_release/.obj
    release:MOC_DIR     = ../bin/_build/_android/manager/_release/.moc
    release:RCC_DIR     = ../bin/_build/_android/manager/_release/.rcc
    release:UI_DIR      = ../bin/_build/_android/manager/_release/.ui
    debug:OBJECTS_DIR   = ../bin/_build/_android/manager/_debug/.obj
    debug:MOC_DIR       = ../bin/_build/_android/manager/_debug/.moc
    debug:RCC_DIR       = ../bin/_build/_android/manager/_debug/.rcc
    debug:UI_DIR        = ../bin/_build/_android/manager/_debug/.ui

    LANGUAGES_TARGET=/assets/languages

} else {
    static: {
    release:OBJECTS_DIR = ../bin/_build/manager/_release/.obj
    release:MOC_DIR     = ../bin/_build/manager/_release/.moc
    release:RCC_DIR     = ../bin/_build/manager/_release/.rcc
    release:UI_DIR      = ../bin/_build/manager/_release/.ui

    debug:OBJECTS_DIR   = ../bin/_build/manager/_debug/.obj
    debug:MOC_DIR       = ../bin/_build/manager/_debug/.moc
    debug:RCC_DIR       = ../bin/_build/manager/_debug/.rcc
    debug:UI_DIR        = ../bin/_build/manager/_debug/.ui
    } else {
    release:OBJECTS_DIR = ../bin/_build/_dynamic/manager/_release/.obj
    release:MOC_DIR     = ../bin/_build/_dynamic/manager/_release/.moc
    release:RCC_DIR     = ../bin/_build/_dynamic/manager/_release/.rcc
    release:UI_DIR      = ../bin/_build/_dynamic/manager/_release/.ui

    debug:OBJECTS_DIR   = ../bin/_build/_dynamic/manager/_debug/.obj
    debug:MOC_DIR       = ../bin/_build/_dynamic/manager/_debug/.moc
    debug:RCC_DIR       = ../bin/_build/_dynamic/manager/_debug/.rcc
    debug:UI_DIR        = ../bin/_build/_dynamic/manager/_debug/.ui
    }

    LANGUAGES_TARGET=$$PWD/../bin/languages
}

INCLUDEPATH += $$PWD $$PWD/_includes "$$PWD/../_Libs" "$$PWD/../_common"

SOURCES += main.cpp\
        mainwindow.cpp \
    common_features/app_path.cpp

HEADERS  += mainwindow.h \
    common_features/app_path.h \
    version.h

FORMS    += mainwindow.ui
