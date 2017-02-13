#
#  Project file for the Qt Creator IDE
#

TEMPLATE = lib
CONFIG  -= qt
CONFIG  += staticlib

TARGET = ogg
INSTALLINCLUDES = $$PWD/include/ogg/*
INSTALLINCLUDESTO = ogg
include($$PWD/../audio_codec_common.pri)

INCLUDEPATH += $$PWD/include

HEADERS += \
    include/ogg/config_types.h \
    include/ogg/ogg.h \
    include/ogg/os_types.h

SOURCES += \
    src/bitwise.c \
    src/framing.c
