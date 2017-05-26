HEADERS += \
    $$PWD/compat.h \
    $$PWD/crc.h \
    $$PWD/debug.h \
    $$PWD/field.h \
    $$PWD/file.h \
    $$PWD/frame.h \
    $$PWD/frametype.h \
    $$PWD/genre.h \
    $$PWD/global.h \
    $$PWD/id3tag.h \
    $$PWD/latin1.h \
    $$PWD/parse.h \
    $$PWD/render.h \
    $$PWD/tag.h \
    $$PWD/ucs4.h \
    $$PWD/utf8.h \
    $$PWD/utf16.h \
    $$PWD/util.h \
    $$PWD/version.h

SOURCES += \
    $$PWD/compat.c \
    $$PWD/crc.c \
    $$PWD/field.c \
    $$PWD/file.c \
    $$PWD/frame.c \
    $$PWD/frametype.c \
    $$PWD/genre.c \
    $$PWD/latin1.c \
    $$PWD/parse.c \
    $$PWD/render.c \
    $$PWD/tag.c \
    $$PWD/ucs4.c \
    $$PWD/utf8.c \
    $$PWD/utf16.c \
    $$PWD/util.c \
    $$PWD/version.c

contains(DEFINES, DEBUG): SOURCES += $$PWD/debug.c
