#/******************************************************************************
#
#gif_lib.h - service library for decoding and encoding GIF images
#
#******************************************************************************/

INCLUDEPATH += $$PWD

SOURCES += \
    $$PWD/dgif_lib.c \
    $$PWD/egif_lib.c \
    $$PWD/gif_err.c \
    $$PWD/gif_font.c \
    $$PWD/gif_hash.c \
    $$PWD/gifalloc.c \
    $$PWD/quantize.c


HEADERS += \
    $$PWD/gif_hash.h \
    $$PWD/gif_lib.h \
    $$PWD/gif_lib_private.h \
    $$PWD/giflib.hpp

