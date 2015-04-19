#-------------------------------------------------
#
# Project created by QtCreator 2015-04-18T11:22:48
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ServerApp
TEMPLATE = app

INCLUDEPATH += $$PWD/../ServerAPI
LIBS += -L$$PWD/../../_Libs/_builds/commonlibs -lServerAPI

DESTDIR = $$PWD/../../bin
    
android:{
    ARCH=android_arm
} else {
    !contains(QMAKE_TARGET.arch, x86_64) {
    ARCH=x32
    } else {
    ARCH=x64
    }
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


SOURCES += main.cpp\
        mainserverwindow.cpp

HEADERS  += mainserverwindow.h

FORMS    += mainserverwindow.ui
