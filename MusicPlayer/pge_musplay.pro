#-------------------------------------------------
#
# Project created by QtCreator 2014-09-19T21:03:41
#
#-------------------------------------------------

!usewinapi:{
    QT  += core gui widgets network
    QT  -= opengl angle svg dbus opengl winextras
} else {
    CONFIG -= qt
}

QTPLUGIN =

!win*-msvc: CONFIG += static

macx: QMAKE_CXXFLAGS += -Wno-header-guard
!macx: !win*-msvc*: {
    QMAKE_LFLAGS += -Wl,-rpath=\'\$\$ORIGIN\'
    LIBS += -lpthread
    QMAKE_LFLAGS_RELEASE += -static-libgcc -static-libstdc++
}

include($$PWD/../_common/strip_garbage.pri)

TEMPLATE = app

include($$PWD/../_common/dest_dir.pri)
include($$PWD/../_common/lib_destdir.pri)

TARGET = pge_musplay

include($$PWD/../_common/build_props.pri)
macx:  TARGET = "PGE Music Player"

CONFIG += c++14
CONFIG += thread

LIBS += -L$$PWD/../_Libs/_builds/$$TARGETOS/lib
INCLUDEPATH += $$PWD/../_Libs/_builds/$$TARGETOS/include

include($$PWD/../_Libs/SDL_Mixer_X/SDL_Mixer_X_link.pri)

win32:{
    RC_FILE = _resources/musicplayer.rc
    win*-msvc*: {
        DEFINES += _CRT_SECURE_NO_WARNINGS
        #QMAKE_CFLAGS_WARN_ON += /wd4244
        QMAKE_CXXFLAGS_WARN_ON += /wd4065 /wd4244
    }

    usewinapi:{
        DEFINES += MUSPLAY_USE_WINAPI
        LIBS += -static -static-libgcc -static-libstdc++ -static -lpthread \
                $$SDL_MIXER_X_LIBS_STATIC -lSDL2 -lSDL2main \
                -lwinmm -lole32 -limm32 -lversion -loleaut32 -luuid -lcomctl32 -mwindows
    } else {
        LIBS += -lSDL2main -lversion $$SDL_MIXER_X_LIBS_DYNAMIC -lcomctl32
        !win*-msvc*: LIBS += -mwindows
        !win*-msvc*: static: {
            QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++ -Wl,-Bdynamic
        }
    }
}
linux-g++||unix:!macx:!android:{
    CONFIG += unversioned_libname
}

macx:{
    ICON = $$PWD/_resources/cat_musplay.icns
    QMAKE_INFO_PLIST = $$PWD/_resources/musplay.plist
    DEFINES += DEFINE_Q_OS_MACX # Workaround for inability of MOC to see this macro
    APP_FILEICON_FILES.files = \
            $$PWD/_resources/file_musplay.icns
    APP_FILEICON_FILES.path  = Contents/Resources
    QMAKE_BUNDLE_DATA += APP_FILEICON_FILES
    LIBS += -lSDL2 $$SDL_MIXER_X_LIBS_STATIC
    LIBS += -framework CoreAudio -framework CoreVideo \
            -framework AudioToolbox -framework AudioUnit \
            -framework IOKit -framework Cocoa -framework Carbon \
            -framework ForceFeedback -framework Metal -framework CoreFoundation
} else {
    !usewinapi:{
        LIBS += -lSDL2 $$SDL_MIXER_X_LIBS_DYNAMIC
        debug: linux-g++: QMAKE_POST_LINK = cp $$PWD/../_Libs/_builds/linux/lib/libSDL2_mixer_ext.so $$DESTDIR
    }
}

usewinapi:{
    SOURCES += MainWindow/musplayer_winapi.cpp
    HEADERS += MainWindow/musplayer_winapi.h
} else {
    SOURCES += MainWindow/musplayer_qt.cpp
    HEADERS += MainWindow/musplayer_qt.h
}

SOURCES += main.cpp\
    SingleApplication/localserver.cpp \
    SingleApplication/singleapplication.cpp \
    wave_writer.c \
    AssocFiles/assoc_files.cpp \
    SingleApplication/pge_application.cpp \
    Effects/reverb.cpp \
    MainWindow/musplayer_base.cpp \
    Player/mus_player.cpp

HEADERS  += \
    SingleApplication/localserver.h \
    SingleApplication/singleapplication.h \
    version.h \
    wave_writer.h \
    AssocFiles/assoc_files.h \
    SingleApplication/pge_application.h \
    Effects/reverb.h \
    defines.h \
    MainWindow/musplayer_base.h \
    Player/mus_player.h

FORMS    += \
    AssocFiles/assoc_files.ui \
    MainWindow/mainwindow.ui

RESOURCES += \
    _resources/musicplayer.qrc
