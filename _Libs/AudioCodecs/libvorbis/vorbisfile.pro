TEMPLATE = lib
CONFIG = staticlib

TARGET = vorbisfile
INSTALLINCLUDES = $$PWD/include/vorbis/vorbisfile.h
INSTALLINCLUDESTO = vorbis
include($$PWD/../audio_codec_common.pri)

DEFINES     += HAVE_CONFIG_H

INCLUDEPATH += $$PWD $$PWD/include $$PWD/lib/

INCLUDEPATH += $$PWD/../libogg/include

HEADERS += \
    include/vorbis/vorbisfile.h

SOURCES += \
    lib/vorbisfile.c
