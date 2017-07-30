HEADERS += \
    $$PWD/music_cmd.h

SOURCES += \
    $$PWD/music_cmd.c

linux-g++: DEFINES += _POSIX_C_SOURCE=1
