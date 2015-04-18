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
    LANGUAGES_TARGET=/assets/languages
    ARCH=android_arm
} else {
    !contains(QMAKE_TARGET.arch, x86_64) {
    ARCH=x32
    } else {
    ARCH=x64
    }
    LANGUAGES_TARGET=$$PWD/../bin/languages
}
static: {
LINKTYPE=static
} else {
LINKTYPE=dynamic
}
debug: BUILDTP=debug
release: BUILDTP=release
OBJECTS_DIR = $$DESTDIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.obj
MOC_DIR     = $$DESTDIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.moc
RCC_DIR     = $$DESTDIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.rcc
UI_DIR      = $$DESTDIR/_build_$$ARCH/$$TARGET/_$$BUILDTP/.ui
message("$$TARGET will be built as $$BUILDTP $$ARCH ($$QMAKE_TARGET.arch) $${LINKTYPE}ally in $$OBJECTS_DIR")

INCLUDEPATH += $$PWD $$PWD/_includes "$$PWD/../_Libs" "$$PWD/../_common"

SOURCES += main.cpp\
        mainwindow.cpp \
    common_features/app_path.cpp

HEADERS  += mainwindow.h \
    common_features/app_path.h \
    version.h

FORMS    += mainwindow.ui
