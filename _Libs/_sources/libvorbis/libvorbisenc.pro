TEMPLATE = lib
CONFIG = staticlib
TARGET = voribsenc

DEFINES     += HAVE_CONFIG_H

INCLUDEPATH += $$PWD $$PWD/include $$PWD/lib/

INCLUDEPATH += $$PWD/../libogg/include

SOURCES += \
    lib/vorbisenc.c
