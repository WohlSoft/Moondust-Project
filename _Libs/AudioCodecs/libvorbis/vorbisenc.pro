#
#  Project file for the Qt Creator IDE
#

TEMPLATE = lib
CONFIG  -= qt
CONFIG  += staticlib

TARGET = vorbisenc
INSTALLINCLUDES = $$PWD/include/vorbis/vorbisenc.h
INSTALLINCLUDESTO = vorbis
include($$PWD/../audio_codec_common.pri)

DEFINES     += HAVE_CONFIG_H

INCLUDEPATH += $$PWD $$PWD/include $$PWD/lib/

INCLUDEPATH += $$PWD/../libogg/include

HEADERS += \
    include/vorbis/vorbisenc.h

SOURCES += \
    lib/vorbisenc.c
