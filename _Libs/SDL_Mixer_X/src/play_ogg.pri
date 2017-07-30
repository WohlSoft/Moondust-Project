
LIBS += -lvorbisfile -lvorbis -logg

HEADERS += \
    $$PWD/load_ogg.h \
    $$PWD/dynamic_ogg.h \
    $$PWD/music_ogg.h

SOURCES += \
    $$PWD/dynamic_ogg.c \
    $$PWD/load_ogg.c \
    $$PWD/music_ogg.c

