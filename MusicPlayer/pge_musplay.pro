#-------------------------------------------------
#
# Project created by QtCreator 2014-09-19T21:03:41
#
#-------------------------------------------------

QT       += core gui widgets network
QT       -= opengl angle svg dbus opengl winextras

QTPLUGIN =

CONFIG += static

macx: QMAKE_CXXFLAGS += -Wno-header-guard
!macx: {
QMAKE_CXXFLAGS += -static -static-libgcc
QMAKE_LFLAGS += -Wl,-rpath=\'\$\$ORIGIN\'
}

include($$PWD/../_common/strip_garbage.pri)

TEMPLATE = app

include($$PWD/../_common/dest_dir.pri)
include($$PWD/../_common/lib_destdir.pri)
TARGET = pge_musplay

include($$PWD/../_common/build_props.pri)

CONFIG += c++11
CONFIG += thread

win32:{
    LIBS += -L$$PWD/../_Libs/_builds/win32/lib
    LIBS += -lSDL2main -lversion -lSDL2_mixer_ext
    INCLUDEPATH += $$PWD/../_Libs/_builds/win32/include
    static: {
        QMAKE_LFLAGS += -static -static-libgcc -static-libstdc++ -Wl,-Bdynamic
    }
}
linux-g++||unix:!macx:!android:{
    LIBS += -L$$PWD/../_Libs/_builds/linux/lib
    INCLUDEPATH += $$PWD/../_Libs/_builds/linux/include
    CONFIG += unversioned_libname
}
android:{
    LIBS += -L$$PWD/../_Libs/_builds/android/lib
    INCLUDEPATH += $$PWD/../_Libs/_builds/android/include

    ANDROID_EXTRA_LIBS += $$PWD/../_Libs/_builds/android/lib/libSDL2.so \
                          $$PWD/../_Libs/_builds/android/lib/libSDL2_mixer_ext.so \
                          $$PWD/../_Libs/_builds/android/lib/libvorbisfile.so \
                          $$PWD/../_Libs/_builds/android/lib/libvorbis.so \
                          $$PWD/../_Libs/_builds/android/lib/libvorbisenc.so \
                          #$$PWD/../_Libs/_builds/android/lib/libvorbisidec.so \
                          $$PWD/../_Libs/_builds/android/lib/libogg.so \
                          $$PWD/../_Libs/_builds/android/lib/libmad.so \
                          $$PWD/../_Libs/_builds/android/lib/libmodplug.so

}

macx:{
    LIBS += -L$$PWD/../_Libs/_builds/macos/lib
    INCLUDEPATH += $$PWD/../_Libs/_builds/macos/include
    INCLUDEPATH += $$PWD/../_Libs/_builds/macos/frameworks/SDL2.framework/Headers
    LIBS += -F$$PWD/../_Libs/_builds/macos/frameworks -framework SDL2 -lSDL2_mixer_ext
    QMAKE_POST_LINK = $$PWD/mac_deploy_libs.sh
} else {
    LIBS += -lSDL2 -lSDL2_mixer_ext
}

RC_FILE = _resources/musicplayer.rc

macx {
    ICON = _resources/cat_musplay.icns
}


SOURCES += main.cpp\
    mainwindow.cpp \
    SingleApplication/localserver.cpp \
    SingleApplication/singleapplication.cpp \
    main_sdl_android.c \
    wave_writer.c

HEADERS  += mainwindow.h \
    SingleApplication/localserver.h \
    SingleApplication/singleapplication.h \
    version.h \
    wave_writer.h

FORMS    += mainwindow.ui

RESOURCES += \
    _resources/musicplayer.qrc

DISTFILES += \
    android/AndroidManifest.xml \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/src/SDLActivity.java

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android


