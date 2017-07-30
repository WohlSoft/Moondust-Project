#
#  Project file for the Qt Creator IDE
#

TEMPLATE = lib
CONFIG  -= qt
CONFIG  += staticlib

TARGET = timidity
INSTALLINCLUDES = $$PWD/include/*
INSTALLINCLUDESTO = timidity
include($$PWD/../audio_codec_common.pri)

exists($$PWD/../../../_common/lib_destdir.pri):{
    include($$PWD/../../../_common/lib_destdir.pri)
    INCLUDEPATH += $$PWD/../../_builds/$$TARGETOS/include/
    LIBS += -L$$PWD/../../_builds/$$TARGETOS/lib/
}

INCLUDEPATH += $$PWD $$PWD/include
LIBS += -lSDL2

HEADERS += \
    include/timidity.h \
    common.h \
    config.h \
    ctrlmode.h \
    filter.h \
    instrum.h \
    mix.h \
    output.h \
    playmidi.h \
    readmidi.h \
    resample.h \
    tables.h \


SOURCES += \
    common.c \
    filter.c \
    instrum.c \
    mix.c \
    output.c \
    playmidi.c \
    readmidi.c \
    sdl_a.c \
    sdl_c.c \
    tables.c \
    timidity.c \
    ctrlmode.c \
    resample_timidity.c

