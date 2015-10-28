#-------------------------------------------------
#
# Project created by QtCreator 2014-09-19T21:03:41
#
#-------------------------------------------------

QT       += core gui widgets network

CONFIG += static

macx: QMAKE_CXXFLAGS += -Wno-header-guard
!macx: {
QMAKE_CXXFLAGS += -static -static-libgcc
QMAKE_LFLAGS += -Wl,-rpath=\'\$\$ORIGIN\'
}

TEMPLATE = app

include(../../_common/dest_dir.pri)
include (../../_common/lib_destdir.pri)
TARGET = DummySDL_Musplay

include(../../_common/build_props.pri)

CONFIG += c++11
CONFIG += thread

win32:{
    LIBS += -L$$PWD/../../_Libs/_builds/win32/lib
    LIBS += -lSDL2main -lversion -lSDL2_mixer_ext
    INCLUDEPATH += $$PWD/../../_Libs/_builds/win32/include
    static: {
        QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++
        LIBS += -lvorbisfile -lvorbis -lmikmod -lmad -lFLAC -logg
    }
}
linux-g++||unix:!macx:!android:{
    LIBS += -L$$PWD/../../_Libs/_builds/linux/lib
    INCLUDEPATH += $$PWD/../../_Libs/_builds/linux/include
    CONFIG += unversioned_libname
}
android:{
    LIBS += -L$$PWD/../../_Libs/_builds/android/lib -lSDL2_mixer_ext
    INCLUDEPATH += $$PWD/../../_Libs/_builds/android/include
}
macx:{
    LIBS += -L$$PWD/../../_Libs/_builds/macos/lib
    INCLUDEPATH += $$PWD/../../_Libs/_builds/macos/include
    INCLUDEPATH += $$PWD/../../_Libs/_Libs/_builds/macos/frameworks/SDL2.framework/Headers
    LIBS += -F$$PWD/../../_Libs/_builds/macos/frameworks -framework SDL2 -lSDL2_mixer_ext
} else {
    LIBS += -lSDL2 -lSDL2_mixer_ext
}

SOURCES += main.cpp\
        mainwindow.cpp \
    SingleApplication/localserver.cpp \
    SingleApplication/singleapplication.cpp

HEADERS  += mainwindow.h \
    SingleApplication/localserver.h \
    SingleApplication/singleapplication.h

FORMS    += mainwindow.ui


