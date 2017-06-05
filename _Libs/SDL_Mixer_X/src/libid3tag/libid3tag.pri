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
    $$PWD/id3_file.c \
    $$PWD/id3_compat.c \
    $$PWD/id3_crc.c \
    $$PWD/id3_debug.c \
    $$PWD/id3_field.c \
    $$PWD/id3_frame.c \
    $$PWD/id3_frametype.c \
    $$PWD/id3_genre.c \
    $$PWD/id3_latin1.c \
    $$PWD/id3_parse.c \
    $$PWD/id3_render.c \
    $$PWD/id3_tag.c \
    $$PWD/id3_ucs4.c \
    $$PWD/id3_utf16.c \
    $$PWD/id3_utf8.c \
    $$PWD/id3_util.c \
    $$PWD/id3_version.c

contains(DEFINES, DEBUG): SOURCES +=
